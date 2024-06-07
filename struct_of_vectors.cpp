#include "input_data.hpp"
#include <vector>
#include <iterator>

struct S
{
  std::vector<int> xs;
  std::vector<int> ys;
  std::vector<int> zs;
  std::vector<int> ds;
};

template <typename P>
void drop_if(S& s, P p)
{
  size_t i = 0;
  while (i < s.xs.size()) {
    if (p(i, s)) {
      s.xs[i] = std::move(s.xs.back());
      s.xs.pop_back();
      s.ys[i] = std::move(s.ys.back());
      s.ys.pop_back();
      s.zs[i] = std::move(s.zs.back());
      s.zs.pop_back();
      s.ds[i] = std::move(s.ds.back());
      s.ds.pop_back();
    }
    else {
      ++i;
    }
  }
}
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
  int iter = 0;
  while (!s.xs.empty()) {
    drop_if(s, [&start](size_t idx, const S& values) { return values.zs[idx] < start; });
    ++iter;
    start += 10;
  }
  return iter;
}
  
