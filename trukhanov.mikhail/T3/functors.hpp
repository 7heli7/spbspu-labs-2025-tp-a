#ifndef FUNCTORS_HPP
#define FUNCTORS_HPP

#include <cstddef>
#include "polygon.hpp"

namespace trukhanov
{
  struct isSize
  {
    size_t size;
    bool operator()(const Polygon& p) const;
  };

  struct CompareByArea
  {
    bool operator()(const Polygon& lhs, const Polygon& rhs) const;
  };

  struct LessArea
  {
    double area;
    bool operator()(const Polygon& p) const;
  };

  struct AccumulateAll
  {
    double operator()(double sum, const Polygon& p) const;
  };

  struct CompareByVertexes
  {
    bool operator()(const Polygon& lhs, const Polygon& rhs) const;
  };

  struct HasDuplicates
  {
    bool operator()(const Polygon& p) const;
  };

  struct PolygonHasMinSize
  {
    bool operator()(const trukhanov::Polygon& p) const;
  };
}

#endif
