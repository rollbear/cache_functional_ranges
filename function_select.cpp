#include <tuple>
#include <vector>
#include <utility>
#include <functional>
#include <cassert>
#include "input_data.hpp"
#include <cstdlib>
#include <ranges>
#include <print>
#include <array>

template <typename ...>
class table;

template <typename, typename>
struct row;

template <typename ... Ts, size_t ... Cs>
struct row<table<Ts...>, std::index_sequence<Cs...>> {
  using row_id = typename table<Ts...>::row_id;
  row(table<Ts...>* tab, size_t offs) : t(tab), offset(offs) {}
  template <size_t ... Is>
  row(const row<table<Ts...>, std::index_sequence<Is...>>& r)
    : t(r.t), offset(r.offset) {}
  row_id id() const { return t->reverse_index_[offset]; }
  template <size_t I>
  friend auto& get(const row& r) {
    return std::get<Cs...[I]>(r.t->data_)[r.offset];
  }
  table<Ts...>* t;
  size_t offset;
};

template <typename Table, size_t ... Cs>
struct std::tuple_size<row<Table, std::index_sequence<Cs...>>> : std::integral_constant<size_t, sizeof...(Cs)> {};

template <size_t I, typename ... Ts, size_t ... Cs>
struct std::tuple_element<I, row<table<Ts...>, std::index_sequence<Cs...>>> {
  using type = Ts...[Cs...[I]]&;
};

template <typename ... Ts>
class table
{
  using columns = std::index_sequence_for<Ts...>;
public:
  using row = ::row<table, columns>;
  template <typename, typename>
  friend struct ::row;
  struct row_id { size_t offset; };
  struct sentinel {};
  struct iterator {
    using value_type = row;
    using difference_type = ssize_t;

    value_type operator*() const {
      return { t, offset };
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
  friend struct iterator;
  iterator begin() { return { this, 0 }; }
  sentinel end() const { return {}; }
  row_id insert(Ts... ts) {
    row_id data_offset { size() };
    auto values = std::forward_as_tuple(ts...);
    std::invoke([&]<size_t ... Cs>(std::index_sequence<Cs...>) {
        (std::get<Cs>(data_).push_back(std::get<Cs>(values)),...);
      },
      columns{});
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
    auto move_last = [&]<size_t ... Cs>(std::index_sequence<Cs...>) {
      (assign_from_last_and_pop_back(std::integral_constant<size_t, Cs>{}),
       ...);
    };
    std::invoke(move_last, columns{});
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
    return { this, offset };
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
    std::invoke([&]<size_t ... Cs>(std::index_sequence<Cs...>) {
        (std::get<Cs>(data_).reserve(size), ...);
      }, columns{});
  }
  
  size_t size() const { return std::get<0>(data_).size(); }
  bool empty() const { return size() == 0; }
private:
  std::tuple<std::vector<Ts>...> data_;
  std::vector<row_id> reverse_index_;
  std::vector<row_id> index_;
  row_id first_free_ = {0};
};
template <size_t ... Is, typename Table, size_t ... Cs>
auto select(const row<Table, std::index_sequence<Cs...>>& r)
{
  return row<Table, std::index_sequence<Cs...[Is]...>>(r);
}

template <typename R, size_t ... Cs>
struct range_selector
{
  using r_iterator = decltype(std::declval<R&>().begin());
  struct iterator : r_iterator {
    using difference_type = ssize_t;
    using value_type = decltype(select<Cs...>(*std::declval<r_iterator>()));
    
    iterator(const r_iterator& i) : r_iterator(i) {}
    auto operator*() const
    {
      const r_iterator& i = *this;
      return select<Cs...>(*i);
    }
  };
  iterator begin() { return iterator{ r.begin() }; }
  auto end() { return r.end(); }
  R& r;
};

template <size_t... Cs> struct range_selector_maker{
  template <typename R>
  friend range_selector<R, Cs...> operator|(R& r, range_selector_maker) {
    return { r };
  }
};

template <size_t ... Cs>
range_selector_maker<Cs...> select() { return {}; }

template <typename>
inline constexpr bool is_row_type_v = false;

template <typename T, typename Cs>
inline constexpr bool is_row_type_v<row<T,Cs>> = true;

template <typename T>
concept row_type = is_row_type_v<T>;

template <size_t ... Cs, typename F>
auto select(F&& f)
  requires (! row_type<std::remove_cvref_t<F>>)
{
  //  return [f = std::forward<F>(f)]<typename T, typename Cs>(row<T, Cs> r)  {
  //    return f(select<Cs...>(r));
  //  };
  return [f = std::forward<F>(f)](row_type auto r)  {
    return f(select<Cs...>(r));
  };
}

template <typename F>
auto apply(F&& f)
{
  return [f = std::forward<F>(f)]<row_type R>(R r) {
    return std::invoke([&]<size_t ... Cs>(std::index_sequence<Cs...>){
        return f(get<Cs>(r)...);
      },
      std::make_index_sequence<std::tuple_size_v<R>>{});
  };
}

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
  values.reserve(static_cast<size_t>(end_data() - begin_data()));
  for (const auto* p = begin_data(); p != end_data(); ++p) {
    values.insert(p->x, p->y, p->z, p->d);
  }
  int start = 100;
  int iter = 0;
  while (!values.empty()) {
    drop_if(values, select<2>(apply([&](auto z) { return z < start; })));
    ++iter;
    start += 10;
  }
  return iter;
}
