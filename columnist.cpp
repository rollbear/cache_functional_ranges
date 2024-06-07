#include <tuple>
#include <vector>
#include <utility>
#include <functional>
#include <cassert>
#include "input_data.hpp"
#include <cstdlib>
#include <columnist/table.hpp>
#include <columnist/functional.hpp>


int main() {
  columnist::table<int,int,int,int> values;
  values.reserve(end_data() - begin_data());
  for (const auto* p = begin_data(); p != end_data(); ++p) {
    values.insert(p->x, p->y, p->z, p->d);
  }

  int start = 100;
  int iter = 0;
  while (!values.empty()) {
    erase_if(values, columnist::select<2>(columnist::apply([start](int z) { return z < start;})));
    ++iter;
    start += 10;
  }
  return iter;
}
