#include "data_struct.hpp"
#include "format_guard.hpp"
#include "wrappers.hpp"
#include <iostream>

std::ostream &nehvedovich::operator<<(std::ostream &out, const data_struct &src)
{
  std::ostream::sentry sentry(out);
  if (!sentry)
  {
    return out;
  }

  FormatGuard fmtguard(out);
  out << "(:" << "key1 " << src.key1; 
  out << "ull:" << "key2 '" << src.key2; 
  out << "':" << "key3 \"" << src.key3;
  out << "\":)";
  return out;
}

std::istream &nehvedovich::operator>>(std::istream &in, data_struct &dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }

  data_struct tmp {};
  std::string key_value {};
  {
    using sep = DelimiterIO;
    in >> sep {'('} >> sep {':'};
    for (int i = 0; i < 3; i++)
    {
      in >> key_value;
      if (key_value == "key1")
      {
        in >> UnsignedLongLongIO {tmp.key1};
      }
      else if (key_value == "key2")
      {
        in >> CharIO {tmp.key2};
      }
      else if (key_value == "key3")
      {
        in >> StringIO {tmp.key3};
      }
      else
      {
        in.setstate(std::ios::failbit);
      }
      in >> sep {':'};
    }
    in >> sep {')'};
  }

  if (in)
  {
    dest = tmp;
  }
  return in;
}

bool nehvedovich::operator<(const data_struct &lhs, const data_struct &rhs)
{
  if (lhs.key1 != rhs.key1)
  {
    return lhs.key1 < rhs.key1;
  }
  if (lhs.key2 != rhs.key2)
  {
    return lhs.key2 < rhs.key2;
  }
  return lhs.key3.size() < rhs.key3.size();
}
