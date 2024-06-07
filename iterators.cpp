#include <tuple>
#include <vector>
#include <utility>
#include <functional>
#include <cassert>
#include "input_data.hpp"
#include <cstdlib>
template <typename ... Ts>
class table
{
public:
  using row = std::tuple<Ts&...>;
  struct row_id { size_t offset; };
  struct sentinel {};
  struct iterator {
    using value_type = row;
    using difference_type = ssize_t;
    using iterator_category = std::forward_iterator_tag;

    value_type operator*() const {
      return std::invoke([&]<size_t ... Is>(std::index_sequence<Is...>){
          return value_type{std::get<Is>(t->data_)[offset]...};
        },
        t->indexes);
    }
    iterator& operator++() {
      ++offset;
      return *this;
    }
    iterator operator++(int) {
      auto copy = *this; ++*this; return copy;
    }
    bool operator==(const iterator&) const = default;
    bool operator==(sentinel) const {
      return offset == t->size();
    }
    table* t;
    size_t offset;
  };
  friend class iterator;
  iterator begin() { return { this, 0 }; }
  sentinel end() const { return {}; }
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
  void erase(iterator i) {
    auto data_offset = i.offset;
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
  void erase(row_id id) {
    auto data_offset = index_[id.offset].offset;
    erase(iterator{this, data_offset});
  }

  row operator[](row_id id) {
    auto offset = index_[id.offset].offset;
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

template <typename V, typename P>
void drop_if(V& v, P p)
{
  for (auto i = v.begin(); i != v.end();) {
    if (p(*i)) {
      v.erase(i);
    } else {
      ++i;
    }
  }
}

int main() {
  table<int,int,int,int> values;
  values.reserve(end_data() - begin_data());
  for (const auto* p = begin_data(); p != end_data(); ++p) {
    values.insert(p->x, p->y, p->z, p->d);
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
