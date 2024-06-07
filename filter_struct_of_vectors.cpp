#include "input_data.hpp"
#include <vector>
#include <iterator>
#include <print>

struct S
{
  std::vector<int> xs;
  std::vector<int> ys;
  std::vector<int> zs;
  std::vector<int> ds;
};

int main()
{
  S s;
  auto size = end_data() - begin_data();
  s.xs.reserve(size);
  s.ys.reserve(size);
  s.zs.reserve(size);
  s.ds.reserve(size);
  for (auto p = begin_data(); p != end_data(); ++p) {
    s.xs.push_back(p->x);
    s.ys.push_back(p->y);
    s.zs.push_back(p->z);
    s.ds.push_back(p->d);
  }

  int start = 100;
  while (start < 50000) {
    auto predicate = [start,&s](size_t idx) {
      return s.ds[idx] == start && s.zs[idx] == 1;
    };
    for (size_t idx = 0; idx != s.xs.size(); ++idx) {
      if (predicate(idx)) {
        std::println("d={} x={}", s.ds[idx], s.xs[idx]);
      }
    }
    ++start;
  }
}
  
