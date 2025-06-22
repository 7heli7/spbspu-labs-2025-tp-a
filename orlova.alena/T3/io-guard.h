#ifndef IO_GUARD_H
#define IO_GUARD_H

#include <iostream>

namespace orlova
{
  class IoGuard
  {
  public:
    explicit IoGuard(std::basic_ios< char >& s);
    ~IoGuard();
  private:
    std::basic_ios< char >& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios< char >::fmtflags fmt_;
  };
}

orlova::IoGuard::IoGuard(std::basic_ios< char >& s) :
  s_(s),
  width_(s.width()),
  fill_(s.fill()),
  precision_(s.precision()),
  fmt_(s.flags())
{}

orlova::IoGuard::~IoGuard()
{
  s_.width(width_);
  s_.fill(fill_);
  s_.precision(precision_);
  s_.flags(fmt_);
}

#endif
