#include "input_data.hpp"
#include <vector>
#include <tuple>
#include <functional>

template <typename ... Ts>
class table
{
public:
  using row = std::tuple<Ts&...>;
  void push_back(Ts... ts) {
    row vs(ts...);
    std::invoke([&]<size_t ... Cs>(std::index_sequence<Cs...>) {
                  (std::get<Cs>(data_).push_back(std::get<Cs>(vs)),...);
                },
                columns);
  }
  void pop_back() {
    std::invoke([&]<size_t ... Cs>(std::index_sequence<Cs...>) {
                  (std::get<Cs>(data_).pop_back(),...);
                },
                columns);
  }
  [[nodiscard]] row operator[](size_t i) {
    auto access = [&]<size_t ... Cs>(std::index_sequence<Cs...>) {
                         return row{std::get<Cs>(data_)[i]...};
    };
    return std::invoke(access, columns);
  }
  [[nodiscard]] row back() {
    return std::invoke([&]<size_t... Cs>(std::index_sequence<Cs...>) {
                         return row{std::get<Cs>(data_).back()...};
                       },
                       columns);
  }
  void reserve(size_t size) {
    std::invoke([&]<size_t ... Cs>(std::index_sequence<Cs...>) {
                  (std::get<Cs>(data_).reserve(size),...);
                },
                columns);
  }
  [[nodiscard]] bool empty() const {
    return std::get<0>(data_).empty();
  }
  [[nodiscard]] size_t size() const {
    return std::get<0>(data_).size();
  }
  
private:
  static constexpr std::index_sequence_for<Ts...> columns{};
  std::tuple<std::vector<Ts>...> data_;
};

template <typename V, typename P>
void drop_if(V& v, P p)
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
  table<int,int,int,int> values;
  values.reserve(end_data() - begin_data());
  for (const auto* p = begin_data(); p != end_data(); ++p) {
    values.push_back(p->x, p->y, p->z, p->d);
  }

  int start = 100;
  int iter = 0;
  while (!values.empty()) {
    drop_if(values, [&](auto r) { auto [x,y,z,d] = r; return z < start; });
    ++iter;
    start += 10;
  }
  return iter;
}
