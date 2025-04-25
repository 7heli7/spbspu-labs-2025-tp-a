#ifndef FORMATGUARD_HPP
#define FORMATGUARD_HPP

#include <ios>

namespace nehvedovich
{
  class FormatGuard
  {
  public:
    explicit FormatGuard(std::basic_ios< char > &s);
    ~FormatGuard();

    FormatGuard(const FormatGuard &) = delete;
    FormatGuard(FormatGuard &&) = delete;
    FormatGuard &operator=(const FormatGuard &) = delete;
    FormatGuard &operator=(FormatGuard &&) = delete;

  private:
    std::basic_ios< char > &s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios< char >::fmtflags fmt_;
  };
} // namespace nehvedovich
#endif
