#include "printCmd.hpp"
#include <map>
#include <functional>
#include <algorithm>
#include <numeric>
#include <iostream>

using plgCmp = std::function< bool(const dribas::Poligon&, const dribas::Poligon&) >;
using plgPrdct = std::function< bool(const dribas::Poligon&) >;

namespace
{
  bool isnEven(const dribas::Poligon& plg)
  {
    return plg.points.size() % 2;
  }

  bool isEven(const dribas::Poligon& plg)
  {
    return !isnEven(plg);
  }

  bool isNumOfVertex(const dribas::Poligon& plg, size_t vertx)
  {
    return plg.points.size() == vertx;
  }

  void printAreaPredicate(const std::vector< dribas::Poligon >& plg, std::ostream& out, const plgPrdct predicate)
  {
    std::vector< dribas::Poligon > temp;
    std::copy_if(plg.cbegin(), plg.cend(), std::back_inserter(temp), predicate);

    std::vector< double > areas;
    std::transform(temp.begin(), temp.end(), std::back_inserter(areas), dribas::getPoligonArea);

    out <<  std::accumulate(areas.begin(), areas.end(), 0);
  }

  bool areaCompare(const dribas::Poligon& lhs, const dribas::Poligon& rhs)
  {
    return dribas::getPoligonArea(lhs) < dribas::getPoligonArea(rhs);
  }

  bool vertexCompare(const dribas::Poligon& lhs, const dribas::Poligon& rhs)
  {
    return lhs.points.size() < rhs.points.size();
  }

  void printNumofVertex(const std::vector< dribas::Poligon >& plg, std::istream& in, std::ostream& out)
  {
    size_t vertex = 0;
    if (!(in >> vertex)) {
      throw std::invalid_argument("Invalid vertex count input");
    }
    printAreaPredicate(plg, out, std::bind(isNumOfVertex, std::placeholders::_1, vertex));
  }

  void printVertexMax(const std::vector< dribas::Poligon >& plgs, std::ostream& out, const plgCmp& compare)
  {
    auto poligon = *std::max_element(plgs.begin(), plgs.end(), compare);
    out << poligon.points.size();
  }

  void printAreaMax(const std::vector< dribas::Poligon >& plgs, std::ostream& out, const plgCmp& compare)
  {
    auto poligon = *std::max_element(plgs.begin(), plgs.end(), compare);
    out << dribas::getPoligonArea(poligon);
  }

  void printVertexMin(const std::vector< dribas::Poligon >& plgs, std::ostream& out, const plgCmp& compare)
  {
  auto poligon = *std::min_element(plgs.begin(), plgs.end(), compare);
    out << poligon.points.size();
  }

  void printAreaMin(const std::vector< dribas::Poligon >& plgs, std::ostream& out, const plgCmp& compare)
  {
    auto poligon = *std::min_element(plgs.begin(), plgs.end(), compare);
    out << dribas::getPoligonArea(poligon);
  }

  void printCountPredicate(const std::vector< dribas::Poligon >& plg, std::ostream& out, const plgPrdct predicate)
  {
    out << std::count_if(plg.cbegin(), plg.cend(), predicate);
  }

  void printCountNumofVertex(const std::vector< dribas::Poligon >& plg, std::istream& in, std::ostream& out)
  {
    size_t vertex = 0;
    if (!(in >> vertex)) {
      throw std::invalid_argument("Invalid vertex count input");
    }
    printCountPredicate(plg, out, std::bind(isNumOfVertex, std::placeholders::_1, vertex));
  }

  struct hasRightAngle
  {
    const dribas::Poligon& polygon;

    bool operator()(size_t i) const 
    {
      size_t n = polygon.points.size();
      size_t prev = (i + n - 1) % n;
      size_t curr = i;
      size_t next = (i + 1) % n;

      const dribas::Point& a = polygon.points[prev];
      const dribas::Point& b = polygon.points[curr];
      const dribas::Point& c = polygon.points[next];

      double ba_x = a.x - b.x;
      double ba_y = a.y - b.y;
      double bc_x = c.x - b.x;
      double bc_y = c.y - b.y;

      double dot = ba_x * bc_x + ba_y * bc_y;
      return std::abs(dot) < 0.0004;
    }

  };

bool hasPlgRightAngle(const dribas::Poligon& plg)
{
  hasRightAngle predicate{plg};
  std::vector< size_t > indices(plg.points.size());
  std::iota(indices.begin(), indices.end(), 0);
  
  return std::any_of(indices.begin(), indices.end(), predicate);
}
}

namespace dribas
{
  void printArea(const std::vector< Poligon >& plg, std::istream& in, std::ostream& out)
  {
    if (!plg.size()) {
      throw std::overflow_error("No figure found");
    }
    std::map< std::string, std::function< void() > > cmds;
    cmds["EVEN"] = std::bind(printAreaPredicate, std::cref(plg), std::ref(out), std::cref(isEven));
    cmds["ODD"] = std::bind(printAreaPredicate, std::cref(plg), std::ref(out), std::cref(isnEven));
    cmds["num-of-vertexes"] = std::bind(printNumofVertex, std::cref(plg), std::ref(in), std::ref(out));

    std::string command; 
    in >> command;
    cmds.at(command)();
  }

  void printMax(const std::vector< Poligon >& plg, std::istream& in, std::ostream& out)
  {
    if (!plg.size()) {
      throw std::overflow_error("NO figure found");
    }
    std::map< std::string, std::function< void() > > cmds;
    cmds["AREA"] = std::bind(printAreaMax, std::cref(plg), std::ref(out), areaCompare);
    cmds["VERTEXES"] = std::bind(printVertexMax, std::cref(plg), std::ref(out), vertexCompare);

    std::string command; 
    in >> command;
    cmds.at(command)();
  }

  void printMin(const std::vector< Poligon >& plg, std::istream& in, std::ostream& out)
  {
    if (!plg.size()) {
      throw std::overflow_error("NO figure found");
    }
    std::map< std::string, std::function< void() > > cmds;
    cmds["AREA"] = std::bind(printAreaMin, std::cref(plg), std::ref(out), areaCompare);
    cmds["VERTEXES"] = std::bind(printVertexMin, std::cref(plg), std::ref(out), vertexCompare);
    std::string command; 
    in >> command;
    cmds.at(command)();
  }

  void printCount(const std::vector< Poligon >& plg, std::istream& in , std::ostream& out)
  {
    if (!plg.size()) {
      throw std::out_of_range("No figure found");
    }
    std::map< std::string, std::function< void() > > cmds;
    cmds["EVEN"] = std::bind(printCountPredicate, std::cref(plg), std::ref(out), std::cref(isEven));
    cmds["ODD"] = std::bind(printCountPredicate, std::cref(plg), std::ref(out), std::cref(isnEven));
    cmds["num-of-vertexes"] = std::bind(printCountNumofVertex, std::cref(plg), std::ref(in), std::ref(out));

    std::string command; 
    in >> command;
    cmds.at(command)();
  }

  void printLessArea(const std::vector< Poligon >& plg, std::istream& in , std::ostream& out)
  {
    if (!plg.size()) {
      throw std::out_of_range("No figure found");
    }
    Poligon temp;
    if (!(in >> temp)) {
      throw std::invalid_argument("Invalid argument for less are poligon");
    }
    printCountPredicate(plg, out, std::bind(areaCompare, std::placeholders::_1, temp));
  }

  void printRightShapes(const std::vector< Poligon >& plg, std::ostream& out)
  {
    out << std::count_if(plg.begin(), plg.end(), hasPlgRightAngle);
  }
}