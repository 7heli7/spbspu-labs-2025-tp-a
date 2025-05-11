#include "polygon.hpp"
#include <algorithm>
#include <cmath>
#include <istream>
#include <iterator>
#include <vector>
#include <numeric>
#include <functional>
#include "scopeGuard.hpp"
#include "delimeters.hpp"

namespace
{
  struct AreaCalculator
  {
    const kiselev::Point* point;
    double operator()(double area, const kiselev::Point& point2)
    {
      area += (point->x * point2.y) - (point2.x * point->y);
      point = &point2;
      return area;
    }
  };

  double distance(const kiselev::Point p1, const kiselev::Point& p2)
  {
    return std::sqrt(std::pow((p1.x - p2.x), 2) + std::pow((p1.y - p2.y), 2));
  }
}
/*
std::istream& kiselev::operator>>(std::istream& in, Point& point)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  detail::ScopeGuard scope(in);
  Point temp;
  in >> DelimeterIO{ '(' } >> temp.x >> DelimeterIO{ ';' } >> temp.y >> DelimeterIO{ ')' };
  if (in)
  {
    point = temp;
  }
  return in;
}

std::istream& kiselev::operator>>(std::istream& in, Polygon& polygon)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  detail::ScopeGuard scope(in);
  size_t count = 0;
  if (!(in >> count) || count < 3)
  {
    in.setstate(std::ios::failbit);
    return in;
  }
  std::vector< Point > temp;
  temp.reserve(count);
  std::copy_n(std::istream_iterator< Point >(in), count, temp.begin());
  if (in)
  {
    polygon.points = std::move(temp);
  }
  return in;
}
*/
std::istream& kiselev::operator>>(std::istream& in, Point& pt)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }

  {
    using sep = DelimeterIO;

    return in >> sep{ '(' } >> pt.x >> sep{ ';' } >> pt.y >> sep{ ')' };
  }
}

std::istream& kiselev::operator>>(std::istream& in, Polygon& plg)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }

  size_t vrtx_cnt;
  if (!(in >> vrtx_cnt) || vrtx_cnt < 3)
  {
    in.setstate(std::ios::failbit);
    return in;
  }

  std::vector< Point > pts(vrtx_cnt);
  std::copy_n(std::istream_iterator< Point >(in), vrtx_cnt, pts.begin());

  if (!in)
  {
    in.setstate(std::ios::failbit);
    return in;
  }

  plg.points = pts;
  return in;
}
double kiselev::getArea(const Polygon& polygon)
{
  AreaCalculator calc{ &polygon.points.back() };
  double area = std::accumulate(polygon.points.begin(), polygon.points.end(), 0.0, std::ref(calc));
  return std::abs(area) / 2.0;
}

bool kiselev::isRect(const Polygon& poly)
{
  if (poly.points.size() != 4)
  {
    return false;
  }
  return (distance(poly.points[0], poly.points[2]) == distance(poly.points[1], poly.points[3]));
}
