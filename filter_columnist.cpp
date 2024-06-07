#include <tuple>
#include <vector>
#include <utility>
#include <functional>
#include <cassert>
#include "input_data.hpp"
#include <cstdlib>
#include <columnist/table.hpp>
#include <columnist/functional.hpp>
#include <ranges>
#include <print>

int main() {
  columnist::table<int,int,int,int> values;
  values.reserve(end_data() - begin_data());
  for (const auto* p = begin_data(); p != end_data(); ++p) {
    values.insert(p->x, p->y, p->z, p->d);
  }

  using columnist::select;
  using columnist::apply;
  int start = 100;
  while (start < 50000) {
    auto predicate = [start](int d, int z) { return d == start && z == 1; };
    auto filter = std::ranges::views::filter(select<3,2>(apply(predicate)));
    auto found_values = values | filter;
    for (auto [x,d] : found_values | columnist::select<0,3>()) {
      std::println("d={} x={}", d, x);
    }
    start += 1;
  }
}
