#include "print_content.hpp"

#include <iostream>
#include <string>

std::ostream& rychkov::operator<<(std::ostream& out, const entities::Variable& var)
{
  print_left(out, var.type);
  out << ' ' << var.name;
  print_right(out, var.type);
  return out;
}
std::ostream& rychkov::operator<<(std::ostream& out, const entities::Function& func)
{
  print_left(out, func.type);
  out << ' ' << func.name;
  out << '(';
  char comma[3] = "\0 ";
  for (size_t i = 0; i < func.type.function_parameters.size(); i++)
  {
    out << comma;
    print_left(out, func.type.function_parameters[i]);
    out << ' ' << func.parameters[i];
    print_right(out, func.type.function_parameters[i]);
    comma[0] = ',';
  }
  out << ')';
  details::print_right_parenthesis(out, func.type);
  print_right(out, *func.type.base);
  return out;
}
std::ostream& rychkov::operator<<(std::ostream& out, const entities::Literal& literal)
{
  switch (literal.type)
  {
  case entities::Literal::String:
    out << '"' << literal.literal << '"';
    break;
  case entities::Literal::Char:
    out << '\'' << literal.literal << '\'';
    break;
  case entities::Literal::Number:
    out << literal.literal;
    break;
  }
  return out << literal.suffix;
}

std::ostream& rychkov::ContentPrinter::indent()
{
  return out << std::string(indent_, '\t');
}
void rychkov::ContentPrinter::print_empty()
{
  indent() << "* [EMPTY]\n";
}
void rychkov::ContentPrinter::operator()(const entities::Variable& var)
{
  indent() << "* [variable] " << var << '\n';
}
void rychkov::ContentPrinter::operator()(const entities::Function& func)
{
  indent() << "* [function] " << func << '\n';
}
void rychkov::ContentPrinter::operator()(const entities::Body& body)
{
  if (body.data.empty())
  {
    indent() << "{}\n";
  }
  else
  {
    indent() << "{\n";
    indent_++;
    for (const entities::Expression& i: body.data)
    {
      operator()(i);
    }
    indent_--;
    indent() << "}\n";
  }
}
void rychkov::ContentPrinter::operator()(const entities::Struct& structure)
{
  indent() << "* [struct] " << structure.name << '\n';
  indent_++;
  for (const entities::Variable& i: structure.fields)
  {
    indent() << "* [field] " << i << '\n';
  }
  indent_--;
}
void rychkov::ContentPrinter::operator()(const entities::Statement& statement)
{
  const char* names[] = {"if", "else", "for", "while", "do", "return", "do-while"};
  indent() << "* [statement] " << names[statement.type] << '\n';
  if ((statement.type != entities::Statement::DO) && (statement.type != entities::Statement::RETURN)
      && (statement.type != entities::Statement::ELSE))
  {
    indent() << "<conditions>:\n";
    indent_++;
    for (size_t i = 0; i < statement.conditions.size(); i++)
    {
      operator()(statement.conditions[i]);
    }
    indent_--;
  }
}
void rychkov::ContentPrinter::operator()(const entities::Enum& structure)
{
  indent() << "* [enum] " << structure.name << '\n';
  indent_++;
  for (const std::pair< std::string, int >& i: structure.fields)
  {
    indent() << i.first << " = " << i.second << '\n';
  }
  indent_--;
}
void rychkov::ContentPrinter::operator()(const entities::Union& structure)
{
  indent() << "* [union] " << structure.name << '\n';
  indent_++;
  for (size_t i = 0; i < structure.fields.size(); i++)
  {
    indent() << "* [variant] " << structure.fields[i] << '\n';
  }
  indent_--;
}
void rychkov::ContentPrinter::operator()(const entities::Alias& alias)
{
  indent() << "* [typedef] ";
  print_left(out, alias.type);
  out << ' ' << alias.name;
  print_right(out, alias.type);
  out << '\n';
}
void rychkov::ContentPrinter::operator()(const entities::Declaration& decl)
{
  if (decl.scope == entities::EXTERN)
  {
    indent() << "* [extern declaration]\n";
  }
  else if (decl.scope == entities::STATIC)
  {
    indent() << "* [static declaration]\n";
  }
  else
  {
    indent() << "* [declaration]\n";
  }
  indent() << "<object>:\n";
  indent_++;
  std::visit(*this, decl.data);
  indent_--;
  if (decl.value != nullptr)
  {
    indent() << "<definition>:\n";
    indent_++;
    operator()(*decl.value);
    indent_--;
  }
}
void rychkov::ContentPrinter::operator()(const entities::Literal& literal)
{
  indent() << "* [literal]\n";
  indent() << "<object> " << literal << '\n';
  indent() << "<type>   " << literal.result_type << '\n';
}
void rychkov::ContentPrinter::operator()(const entities::CastOperation& cast)
{
  indent() << (cast.is_explicit ? "[explicit cast]\n" : "[implicit cast]\n");
  indent() << "<to>   " << cast.to << '\n';
  indent() << "<object>:\n";
  indent_++;
  operator()(cast.expr);
  indent_--;
}
