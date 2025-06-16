#include "type_parser.hpp"

#include <map>

bool rychkov::TypeParser::append(CParseContext& context, char c)
{
  using append_signature = bool(TypeParser::*)(CParseContext&);
  using append_map = std::map< char, append_signature >;
  static const append_map dispatch_map = {
        {'*', &TypeParser::append_asterisk},
        {'[', &TypeParser::append_open_bracket},
        {']', &TypeParser::append_close_bracket},
        {'(', &TypeParser::append_open_parenthesis},
        {')', &TypeParser::append_close_parenthesis},
        {',', &TypeParser::append_comma}
      };

  append_map::const_iterator found = dispatch_map.find(c);
  if (found != dispatch_map.cend())
  {
    return (this->*(found->second))(context);
  }
  log(context, "unknown symbol in type declaration");
  return false;
}
bool rychkov::TypeParser::append_asterisk(CParseContext& context)
{
  if (stack_.empty() || stack_.top().right_allign)
  {
    log(context, "pointer cannot be applied here");
    return false;
  }
  if (!stack_.top().data->empty())
  {
    move_up();
  }
  stack_.top().data->category = typing::Type::Pointer;
  return true;
}
bool rychkov::TypeParser::append_open_bracket(CParseContext& context)
{
  if (stack_.top().data->empty() || stack_.top().bracket_opened
      || (stack_.top().data->category == typing::Type::Function))
  {
    log(context, "array cannot be applied here");
    return false;
  }
  typing::Type* old = move_up();
  stack_.top().data->category = typing::Type::Array;
  if (old->category == typing::Type::Array)
  {
    stack_.top().data->array_has_length = old->array_has_length;
    stack_.top().data->array_length = old->array_length;
    old->array_has_length = false;
    old->array_length = 0;
    stack_.top().data = old;
    stack_.top().array_must_have_size = true;
  }
  stack_.top().bracket_opened = true;
  stack_.top().right_allign = true;
  return true;
}
bool rychkov::TypeParser::append_close_bracket(CParseContext& context)
{
  if (stack_.empty() || !stack_.top().bracket_opened)
  {
    log(context, "found unpaired ']'");
    return false;
  }
  if (stack_.top().array_must_have_size && !stack_.top().data->array_has_length)
  {
    log(context, "array cannot be abstract here");
    return false;
  }
  stack_.top().bracket_opened = false;
  return true;
}
bool rychkov::TypeParser::append_open_parenthesis(CParseContext& context)
{
  if (stack_.top().data->empty())
  {
    log(context, "type cannot starts with '('");
    return false;
  }
  if (stack_.top().right_allign)
  {
    if ((stack_.top().data->category == typing::Type::Array)
        || (stack_.top().data->category == typing::Type::Function))
    {
      log(context, "function parentheses cannot be applied here");
      return false;
    }
    if (!stack_.top().data->empty())
    {
      move_up();
    }
    stack_.top().data->category = typing::Type::Function;
    stack_.top().data->function_parameters.emplace_back();
    stack_.push({&stack_.top().data->function_parameters.back()});
    stack_.top().is_function_paremeter = true;
    if (can_be_named_param(stack_.top().data))
    {
      parameters_.emplace_back();
    }
    return true;
  }
  typing::Type* old = move_up();
  std::swap(old, stack_.top().data);
  stack_.push({old});
  stack_.top().parenthesis_opened = true;
  return true;
}
bool rychkov::TypeParser::append_close_parenthesis(CParseContext& context)
{
  if (stack_.size() <= 1)
  {
    log(context, "found unpaired ')'");
    return false;
  }
  if (stack_.top().is_function_paremeter)
  {
    if (stack_.top().data->empty())
    {
      stack_.pop();
      if (stack_.top().data->function_parameters.size() == 1)
      {
        if (can_be_named_param(&stack_.top().data->function_parameters[0]))
        {
          parameters_.clear();
        }
        stack_.top().data->function_parameters.clear();
        return true;
      }
      log(context, "function parameter following after ',' cannot be empty");
      return false;
    }
    stack_.pop();
    return true;
  }
  if (stack_.top().bracket_opened)
  {
    log(context, "')' cannot follow '['");
    return false;
  }
  if (stack_.top().data->empty())
  {
    decltype(typing::Type::base) temp = std::move(stack_.top().data->base);
    *stack_.top().data = std::move(*temp);

    typing::Type* old = stack_.top().data;
    stack_.pop();
    stack_.top().data = old;
    stack_.top().right_allign = true;
    return true;
  }
  stack_.pop();
  stack_.top().right_allign = true;
  return true;
}
bool rychkov::TypeParser::append_comma(CParseContext& context)
{
  if ((stack_.size() <= 1) || !stack_.top().is_function_paremeter)
  {
    log(context, "',' cannot be applied here");
    return false;
  }
  stack_.pop();
  stack_.top().data->function_parameters.emplace_back();
  stack_.push({&stack_.top().data->function_parameters.back()});
  stack_.top().is_function_paremeter = true;
  if (can_be_named_param(stack_.top().data))
  {
    parameters_.emplace_back();
  }
  return true;
}
