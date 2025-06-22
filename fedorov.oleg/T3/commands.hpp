#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include "polygon.hpp"

namespace fedorov
{
  void areaCommand(std::istream &, std::ostream &, const std::vector< Polygon > &);
  void lessAreaCommand(std::istream &, std::ostream &, const std::vector< Polygon > &);
  void maxCommand(std::istream &, std::ostream &, const std::vector< Polygon > &);
  void minCommand(std::istream &, std::ostream &, const std::vector< Polygon > &);
  void echoCommand(std::istream &, std::ostream &, std::vector< Polygon > &);
  void countCommand(std::istream &, std::ostream &, const std::vector< Polygon > &);
}

#endif
