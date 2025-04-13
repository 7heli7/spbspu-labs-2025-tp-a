#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include "DataStruct.h"
int main()
{
  std::vector<khokhryakova::DataStruct> data;
  while (!std::cin.eof())
  {
    khokhryakova::DataStruct temp;
    if (std::cin >> temp)
    {
      data.push_back(temp);
    }
    else
    {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }
  std::sort(data.begin(), data.end());
  std::copy(
    data.begin(),
    data.end(),
    std::ostream_iterator<khokhryakova::DataStruct>(std::cout, "\n")
  );
  return 0;
}
