#include "lexer.hpp"

#include <iostream>
#include <cctype>
#include <utility>
#include "cparser.hpp"
#include "print_content.hpp"

rychkov::Lexer::Lexer(CParser* next):
  next_{next},
  literal_full_{false},
  screened_{false}
{}

const std::set< std::vector< rychkov::Operator >, rychkov::NameCompare > rychkov::Lexer::cases = {
      {
        {rychkov::Operator::unary, rychkov::Operator::arithmetic, "+", false, true, false, 2},
        {rychkov::Operator::binary, rychkov::Operator::arithmetic, "+", false, false, false, 4}
      },
      {
        {rychkov::Operator::unary, rychkov::Operator::arithmetic, "++", true, false, false, 1},
        {rychkov::Operator::unary, rychkov::Operator::arithmetic, "++", true, true, true, 2},
      },
      {
        {rychkov::Operator::unary, rychkov::Operator::arithmetic, "-", false, true, false, 2},
        {rychkov::Operator::binary, rychkov::Operator::arithmetic, "-", false, false, false, 4}
      },
      {
        {rychkov::Operator::unary, rychkov::Operator::arithmetic, "--", true, false, false, 1},
        {rychkov::Operator::unary, rychkov::Operator::arithmetic, "--", true, true, true, 2},
      },
      {{rychkov::Operator::unary, rychkov::Operator::special, "!", false, true, false, 2}},
      {
        {rychkov::Operator::unary, rychkov::Operator::arithmetic, "*", false, true, false, 2},
        {rychkov::Operator::binary, rychkov::Operator::arithmetic, "*", false, false, false, 3}
      },
      {{rychkov::Operator::binary, rychkov::Operator::arithmetic, "*=", true, true, true, 14}},
      {{rychkov::Operator::binary, rychkov::Operator::assign, "=", true, true, true, 14}},
      {{rychkov::Operator::binary, rychkov::Operator::logic, "==", false, false, false, 7}}
    };

void rychkov::Lexer::parse(CParseContext& context, std::string str)
{
  for (char c: str)
  {
    append(context, c);
    context.symbol++;
    if (c == '\n')
    {
      context.line++;
      context.symbol = 0;
    }
  }
}
void rychkov::Lexer::flush(CParseContext& context)
{
  if (std::holds_alternative< std::string >(buf_))
  {
    if (next_ == nullptr)
    {
      context.out << "<name> " << std::get< std::string >(buf_) << '\n';
    }
    else
    {
      next_->append(context, std::move(std::get< std::string >(buf_)));
    }
  }
  else if (std::holds_alternative< entities::Literal >(buf_))
  {
    entities::Literal& lit = std::get< entities::Literal >(buf_);
    if (lit.type == entities::Literal::Number)
    {
      if ((lit.suffix == "l") || (lit.suffix == "L"))
      {
        lit.result_type.length_category = typing::Type::LONG;
      }
      else if ((lit.suffix == "ul") || (lit.suffix == "UL"))
      {
        lit.result_type.length_category = typing::Type::LONG;
        lit.result_type.is_unsigned = true;
      }
      else if ((lit.suffix == "ll") || (lit.suffix == "LL"))
      {
        lit.result_type.length_category = typing::Type::LONG_LONG;
      }
      else if ((lit.suffix == "ull") || (lit.suffix == "ULL"))
      {
        lit.result_type.length_category = typing::Type::LONG_LONG;
        lit.result_type.is_unsigned = true;
      }
      else if (!lit.suffix.empty())
      {
        log(context, "unknown numeric suffix - " + lit.suffix);
        lit.suffix.clear();
      }
    }
    else if (!lit.suffix.empty())
    {
      log(context, "unknown literal suffix - " + lit.suffix);
      lit.suffix.clear();
    }

    if (next_ == nullptr)
    {
      context.out << "<lit>  " << lit << '\n';
    }
    else
    {
      next_->append(context, lit);
    }
  }
  else if (std::holds_alternative< operator_value >(buf_))
  {
    if (next_ == nullptr)
    {
      context.out << "<oper> " << (*std::get< operator_value >(buf_))[0].token << '\n';
    }
    else
    {
      next_->append(context, *std::get< operator_value >(buf_));
    }
  }
  literal_full_ = false;
  buf_.emplace< 0 >();
}
void rychkov::Lexer::append(CParseContext& context, char c)
{
  if (std::holds_alternative< entities::Literal >(buf_))
  {
    append_literal(context, c);
  }
  else if (std::holds_alternative< std::string >(buf_))
  {
    append_name(context, c);
  }
  else if (std::holds_alternative< operator_value >(buf_))
  {
    append_operator(context, c);
  }
  else
  {
    append_new(context, c);
  }
}
void rychkov::Lexer::append_new(CParseContext& context, char c)
{
  decltype(cases)::const_iterator oper_p = cases.find(std::string{c});
  if (oper_p != cases.end())
  {
    buf_ = &*oper_p;
  }
  else if (std::isalpha(c))
  {
    buf_ = std::string{c};
  }
  else if (std::isdigit(c))
  {
    buf_ = entities::Literal{{c}, {}, entities::Literal::Number, {"int", typing::Type::Int}};
  }
  else if (c == '\'')
  {
    buf_ = entities::Literal{{}, {}, entities::Literal::Char, {"char", typing::Type::Int}};
  }
  else if (c == '"')
  {
    typing::Type type = {{}, typing::Type::Array, {"char", typing::Type::Int, nullptr, true}};
    buf_ = entities::Literal{{}, {}, entities::Literal::String, std::move(type)};
  }
  else if (!std::isspace(c))
  {
    if (next_ == nullptr)
    {
      context.out << "<spec> " << c << '\n';
      return;
    }
    next_->append(context, c);
  }
}
void rychkov::Lexer::append_name(CParseContext& context, char c)
{
  std::string& str = std::get< std::string >(buf_);
  if (std::isalnum(c))
  {
    str += c;
    return;
  }
  flush(context);
  append(context, c);
}
void rychkov::Lexer::append_literal(CParseContext& context, char c)
{
  entities::Literal& literal = std::get< entities::Literal >(buf_);
  if (std::isalnum(c))
  {
    if ((literal.type == entities::Literal::Number) && !literal_full_ && std::isalpha(c))
    {
      if ((literal.literal == "0x") || (literal.literal == "0b"))
      {
        log(context, "wrong numeric literal prefix");
        return;
      }
      if ((literal.literal != "0") || ((c != 'x') && (c != 'b')))
      {
        literal_full_ = true;
      }
    }
    (literal_full_ ? literal.suffix : literal.literal) += c;
    return;
  }
  if (literal_full_)
  {
    flush(context);
    append(context, c);
    return;
  }
  switch (literal.type)
  {
  case entities::Literal::String:
    if (c == '"')
    {
      literal_full_ = true;
      return;
    }
    literal.literal += c;
    return;
  case entities::Literal::Char:
    if (c == '\'')
    {
      literal_full_ = true;
      return;
    }
    literal.literal += c;
    return;
  case entities::Literal::Number:
    if (c == '.')
    {
      literal.literal += c;
      return;
    }
    if (c != '\'')
    {
      flush(context);
      append(context, c);
    }
    return;
  }
  log(context, "unexpected symbol");
}
void rychkov::Lexer::append_operator(CParseContext& context, char c)
{
  operator_value& oper = std::get< operator_value >(buf_);
  decltype(cases)::const_iterator oper_p = cases.find((*oper)[0].token + c);
  if (oper_p != cases.cend())
  {
    buf_ = &*oper_p;
    return;
  }
  flush(context);
  append(context, c);
}
