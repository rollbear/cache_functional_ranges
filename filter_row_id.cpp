#include <tuple>
#include <vector>
#include <utility>
#include <functional>
#include <cassert>
#include "input_data.hpp"
#include <cstdlib>
#include <print>

template <typename ... Ts>
class table
{
public:
  using row = std::tuple<Ts&...>;
  struct row_id { size_t offset; };
  row_id insert(Ts... ts) {
    row_id data_offset { size() };
    row values(ts...);
    std::invoke([&]<size_t ... Is>(std::index_sequence<Is...>) {
        (std::get<Is>(data_).push_back(std::get<Is>(values)),...);
      },
      indexes);
    if (first_free_.offset == index_.size()) {
      reverse_index_.push_back(first_free_);
      index_.push_back(data_offset);
      first_free_.offset = index_.size();
      return data_offset;
    } else {
      auto new_pos = std::exchange(first_free_, index_[first_free_.offset]);
      index_[new_pos.offset] = data_offset;
      reverse_index_.push_back(new_pos);
      return new_pos;
    }
  }
  void erase(row_id id) {
    auto data_offset = index_[id.offset].offset;
    erase(data_offset);
  }
  void erase(size_t data_offset) {
    auto id = reverse_index_[data_offset];
    auto assign_from_last_and_pop_back = [this, data_offset](auto i) {
      auto& vec = std::get<i.value>(data_);
      vec[data_offset] = vec.back();
      vec.pop_back();
    };
    auto move_last = [&]<size_t ... Is>(std::index_sequence<Is...>) {
      (assign_from_last_and_pop_back(std::integral_constant<size_t, Is>{}),
       ...);
    };
    std::invoke(move_last, indexes);
    if (data_offset != reverse_index_.size() - 1) {
      reverse_index_[data_offset] = reverse_index_.back();
      index_[reverse_index_[data_offset].offset].offset = data_offset;
    }
    reverse_index_.pop_back();
    index_[id.offset] = first_free_;
    first_free_ = id;
  }
  row operator[](row_id id) {
    auto offset = index_[id.offset].offset;
    return (*this)[offset];
  }
  row operator[](size_t offset) {
    auto access = [&]<size_t ... Is>(std::index_sequence<Is...>) {
      return row{std::get<Is>(data_)[offset]...};
    };
    return std::invoke(access, indexes);
  }
  bool has_row(row_id id) const {
    if (id.offset >= index_.size()) {
      return false;
    }
    auto offset = index_[id.offset].offset;
    if (offset >= reverse_index_.size()) {
      return false;
    }
    return reverse_index_[offset].offset == id.offset;
  }
  void reserve(size_t size) {
    std::invoke([&]<size_t ... Is>(std::index_sequence<Is...>) {
        (std::get<Is>(data_).reserve(size), ...);
      }, indexes);
  }
  
  size_t size() const { return std::get<0>(data_).size(); }
  bool empty() const { return size() == 0; }
private:
  static constexpr auto indexes = std::index_sequence_for<Ts...>{};
  std::tuple<std::vector<Ts>...> data_;
  std::vector<row_id> reverse_index_;
  std::vector<row_id> index_;
  row_id first_free_ = {0};
};


int main()
{
  table<int,int,int,int> values;
  values.reserve(end_data() - begin_data());
  for (const auto* p = begin_data(); p != end_data(); ++p) {
    values.insert(p->x, p->y, p->z, p->d);
  }

  int start = 0;
  while (start < 50000) {
    for (size_t i = 0; i != values.size(); ++i) {
      auto [x,y,z,d] = values[i];
      if (d == start && z == 1) {
        std::println("d={} x={}", d, x);
      }
    }
    ++start;
  }
}
