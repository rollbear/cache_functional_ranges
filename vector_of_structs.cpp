#include "input_data.hpp"
#include <vector>

template <typename T, typename P>
void drop_if(std::vector<T>& v, P p)
{
  size_t i = 0;
  while (i < v.size()) {
    if (p(v[i])) {
      v[i] = std::move(v.back());
      v.pop_back();
    }
    else {
      ++i;
    }
  }
}

int main()
{
  std::vector<object> values(begin_data(), end_data());

  int start = 100;
  int iter = 0;
  while (!values.empty()) {
    drop_if(values, [&start](const auto& obj) { return obj.z < start; });
    ++iter;
    start += 10;
  }
  return iter;
}
