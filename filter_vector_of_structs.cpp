#include "input_data.hpp"
#include <vector>
#include <ranges>
#include <print>
int main()
{
  std::vector<object> values(begin_data(), end_data());

  int start = 100;
  while (start < 50000) {
    auto predicate = [start](auto r) {
      return r.d == start && r.z == 1;
    };
    auto found_values = values | std::ranges::views::filter(predicate);
    for (auto [x,y,z,d] : found_values) {
      std::println("d={} x={}", d, x);
    }
    ++start;
  }
}
