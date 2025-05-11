#ifndef PRINTCMD_HPP
#define PRINTCMD_HPP

#include <iostream>
#include <vector>
#include "poligon.hpp"

namespace dribas
{
  void printArea(const std::vector< Poligon >&, std::istream&, std::ostream&);
  void printMax(const std::vector< Poligon >&, std::istream&, std::ostream&);
  void printMin(const std::vector< Poligon >&, std::istream&, std::ostream&);
  void printCount(const std::vector< Poligon >&, std::istream&, std::ostream&);
  void printLessArea(const std::vector< Poligon >&, std::istream&, std::ostream&);
  void printLessArea(const std::vector< Poligon >&, std::istream&, std::ostream&);
}

#endif
