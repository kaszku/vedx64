#include "vedx64_bridge.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#if __cplusplus >= 201703L
#include <string_view>
#endif
#if __cplusplus >= 202002L
#include <ranges>
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#ifdef __clang__
#pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif // __clang__
#endif // __GNUC__

namespace rust {
inline namespace cxxbridge1 {
// #include "rust/cxx.h"

#ifndef CXXBRIDGE1_PANIC
#define CXXBRIDGE1_PANIC
template <typename Exception>
void panic [[noreturn]] (const char *msg);
#endif // CXXBRIDGE1_PANIC

struct unsafe_bitcopy_t;

namespace {
template <typename T>
class impl;
} // namespace

class Opaque;

template <typename T>
::std::size_t size_of();
template <typename T>
::std::size_t align_of();

#ifndef CXXBRIDGE1_RUST_STRING
#define CXXBRIDGE1_RUST_STRING
class String final {
public:
  String() noexcept;
  String(const String &) noexcept;
  String(String &&) noexcept;
  ~String() noexcept;

  String(const std::string &);
  String(const char *);
  String(const char *, std::size_t);
  String(const char16_t *);
  String(const char16_t *, std::size_t);
#ifdef __cpp_char8_t
  String(const char8_t *s);
  String(const char8_t *s, std::size_t len);
#endif

  static String lossy(const std::string &) noexcept;
  static String lossy(const char *) noexcept;
  static String lossy(const char *, std::size_t) noexcept;
  static String lossy(const char16_t *) noexcept;
  static String lossy(const char16_t *, std::size_t) noexcept;

  String &operator=(const String &) & noexcept;
  String &operator=(String &&) & noexcept;

  explicit operator std::string() const;

  const char *data() const noexcept;
  std::size_t size() const noexcept;
  std::size_t length() const noexcept;
  bool empty() const noexcept;

  const char *c_str() noexcept;

  std::size_t capacity() const noexcept;
  void reserve(size_t new_cap) noexcept;

  using iterator = char *;
  iterator begin() noexcept;
  iterator end() noexcept;

  using const_iterator = const char *;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  bool operator==(const String &) const noexcept;
  bool operator!=(const String &) const noexcept;
  bool operator<(const String &) const noexcept;
  bool operator<=(const String &) const noexcept;
  bool operator>(const String &) const noexcept;
  bool operator>=(const String &) const noexcept;

  void swap(String &) noexcept;

  String(unsafe_bitcopy_t, const String &) noexcept;

private:
  struct lossy_t;
  String(lossy_t, const char *, std::size_t) noexcept;
  String(lossy_t, const char16_t *, std::size_t) noexcept;
  friend void swap(String &lhs, String &rhs) noexcept { lhs.swap(rhs); }

  std::array<std::uintptr_t, 3> repr;
};
#endif // CXXBRIDGE1_RUST_STRING

#ifndef CXXBRIDGE1_RUST_STR
#define CXXBRIDGE1_RUST_STR
class Str final {
public:
  Str() noexcept;
  Str(const String &) noexcept;
  Str(const std::string &);
  Str(const char *);
  Str(const char *, std::size_t);

  Str &operator=(const Str &) & noexcept = default;

  explicit operator std::string() const;
#if __cplusplus >= 201703L
  explicit operator std::string_view() const;
#endif

  const char *data() const noexcept;
  std::size_t size() const noexcept;
  std::size_t length() const noexcept;
  bool empty() const noexcept;

  Str(const Str &) noexcept = default;
  ~Str() noexcept = default;

  using iterator = const char *;
  using const_iterator = const char *;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  bool operator==(const Str &) const noexcept;
  bool operator!=(const Str &) const noexcept;
  bool operator<(const Str &) const noexcept;
  bool operator<=(const Str &) const noexcept;
  bool operator>(const Str &) const noexcept;
  bool operator>=(const Str &) const noexcept;

  void swap(Str &) noexcept;

private:
  class uninit;
  Str(uninit) noexcept;
  friend impl<Str>;

  std::array<std::uintptr_t, 2> repr;
};
#endif // CXXBRIDGE1_RUST_STR

#ifndef CXXBRIDGE1_RUST_SLICE
#define CXXBRIDGE1_RUST_SLICE
namespace detail {
template <bool>
struct copy_assignable_if {};

template <>
struct copy_assignable_if<false> {
  copy_assignable_if() noexcept = default;
  copy_assignable_if(const copy_assignable_if &) noexcept = default;
  copy_assignable_if &operator=(const copy_assignable_if &) & noexcept = delete;
  copy_assignable_if &operator=(copy_assignable_if &&) & noexcept = default;
};
} // namespace detail

template <typename T>
class Slice final
    : private detail::copy_assignable_if<std::is_const<T>::value> {
public:
  using value_type = T;

  Slice() noexcept;
  Slice(T *, std::size_t count) noexcept;

  template <typename C>
  explicit Slice(C &c) : Slice(c.data(), c.size()) {}

  Slice &operator=(const Slice<T> &) & noexcept = default;
  Slice &operator=(Slice<T> &&) & noexcept = default;

  T *data() const noexcept;
  std::size_t size() const noexcept;
  std::size_t length() const noexcept;
  bool empty() const noexcept;

  T &operator[](std::size_t n) const noexcept;
  T &at(std::size_t n) const;
  T &front() const noexcept;
  T &back() const noexcept;

  Slice(const Slice<T> &) noexcept = default;
  ~Slice() noexcept = default;

  class iterator;
  iterator begin() const noexcept;
  iterator end() const noexcept;

  void swap(Slice &) noexcept;

private:
  class uninit;
  Slice(uninit) noexcept;
  friend impl<Slice>;
  friend void sliceInit(void *, const void *, std::size_t) noexcept;
  friend void *slicePtr(const void *) noexcept;
  friend std::size_t sliceLen(const void *) noexcept;

  std::array<std::uintptr_t, 2> repr;
};

#ifdef __cpp_deduction_guides
template <typename C>
explicit Slice(C &c)
    -> Slice<std::remove_reference_t<decltype(*std::declval<C>().data())>>;
#endif // __cpp_deduction_guides

template <typename T>
class Slice<T>::iterator final {
public:
#if __cplusplus >= 202002L
  using iterator_category = std::contiguous_iterator_tag;
#else
  using iterator_category = std::random_access_iterator_tag;
#endif
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = typename std::add_pointer<T>::type;
  using reference = typename std::add_lvalue_reference<T>::type;

  reference operator*() const noexcept;
  pointer operator->() const noexcept;
  reference operator[](difference_type) const noexcept;

  iterator &operator++() noexcept;
  iterator operator++(int) noexcept;
  iterator &operator--() noexcept;
  iterator operator--(int) noexcept;

  iterator &operator+=(difference_type) noexcept;
  iterator &operator-=(difference_type) noexcept;
  iterator operator+(difference_type) const noexcept;
  friend inline iterator operator+(difference_type lhs, iterator rhs) noexcept {
    return rhs + lhs;
  }
  iterator operator-(difference_type) const noexcept;
  difference_type operator-(const iterator &) const noexcept;

  bool operator==(const iterator &) const noexcept;
  bool operator!=(const iterator &) const noexcept;
  bool operator<(const iterator &) const noexcept;
  bool operator<=(const iterator &) const noexcept;
  bool operator>(const iterator &) const noexcept;
  bool operator>=(const iterator &) const noexcept;

private:
  friend class Slice;
  void *pos;
  std::size_t stride;
};

#if __cplusplus >= 202002L
static_assert(std::ranges::contiguous_range<rust::Slice<const uint8_t>>);
static_assert(std::contiguous_iterator<rust::Slice<const uint8_t>::iterator>);
#endif

template <typename T>
Slice<T>::Slice() noexcept {
  sliceInit(this, reinterpret_cast<void *>(align_of<T>()), 0);
}

template <typename T>
Slice<T>::Slice(T *s, std::size_t count) noexcept {
  assert(s != nullptr || count == 0);
  sliceInit(this,
            s == nullptr && count == 0
                ? reinterpret_cast<void *>(align_of<T>())
                : const_cast<typename std::remove_const<T>::type *>(s),
            count);
}

template <typename T>
T *Slice<T>::data() const noexcept {
  return reinterpret_cast<T *>(slicePtr(this));
}

template <typename T>
std::size_t Slice<T>::size() const noexcept {
  return sliceLen(this);
}

template <typename T>
std::size_t Slice<T>::length() const noexcept {
  return this->size();
}

template <typename T>
bool Slice<T>::empty() const noexcept {
  return this->size() == 0;
}

template <typename T>
T &Slice<T>::operator[](std::size_t n) const noexcept {
  assert(n < this->size());
  auto ptr = static_cast<char *>(slicePtr(this)) + size_of<T>() * n;
  return *reinterpret_cast<T *>(ptr);
}

template <typename T>
T &Slice<T>::at(std::size_t n) const {
  if (n >= this->size()) {
    panic<std::out_of_range>("rust::Slice index out of range");
  }
  return (*this)[n];
}

template <typename T>
T &Slice<T>::front() const noexcept {
  assert(!this->empty());
  return (*this)[0];
}

template <typename T>
T &Slice<T>::back() const noexcept {
  assert(!this->empty());
  return (*this)[this->size() - 1];
}

template <typename T>
typename Slice<T>::iterator::reference
Slice<T>::iterator::operator*() const noexcept {
  return *static_cast<T *>(this->pos);
}

template <typename T>
typename Slice<T>::iterator::pointer
Slice<T>::iterator::operator->() const noexcept {
  return static_cast<T *>(this->pos);
}

template <typename T>
typename Slice<T>::iterator::reference Slice<T>::iterator::operator[](
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ptr = static_cast<char *>(this->pos) + this->stride * n;
  return *reinterpret_cast<T *>(ptr);
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator++() noexcept {
  this->pos = static_cast<char *>(this->pos) + this->stride;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator++(int) noexcept {
  auto ret = iterator(*this);
  this->pos = static_cast<char *>(this->pos) + this->stride;
  return ret;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator--() noexcept {
  this->pos = static_cast<char *>(this->pos) - this->stride;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator--(int) noexcept {
  auto ret = iterator(*this);
  this->pos = static_cast<char *>(this->pos) - this->stride;
  return ret;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator+=(
    typename Slice<T>::iterator::difference_type n) noexcept {
  this->pos = static_cast<char *>(this->pos) + this->stride * n;
  return *this;
}

template <typename T>
typename Slice<T>::iterator &Slice<T>::iterator::operator-=(
    typename Slice<T>::iterator::difference_type n) noexcept {
  this->pos = static_cast<char *>(this->pos) - this->stride * n;
  return *this;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator+(
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ret = iterator(*this);
  ret.pos = static_cast<char *>(this->pos) + this->stride * n;
  return ret;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::iterator::operator-(
    typename Slice<T>::iterator::difference_type n) const noexcept {
  auto ret = iterator(*this);
  ret.pos = static_cast<char *>(this->pos) - this->stride * n;
  return ret;
}

template <typename T>
typename Slice<T>::iterator::difference_type
Slice<T>::iterator::operator-(const iterator &other) const noexcept {
  auto diff = std::distance(static_cast<char *>(other.pos),
                            static_cast<char *>(this->pos));
  return diff / static_cast<typename Slice<T>::iterator::difference_type>(
                    this->stride);
}

template <typename T>
bool Slice<T>::iterator::operator==(const iterator &other) const noexcept {
  return this->pos == other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator!=(const iterator &other) const noexcept {
  return this->pos != other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator<(const iterator &other) const noexcept {
  return this->pos < other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator<=(const iterator &other) const noexcept {
  return this->pos <= other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator>(const iterator &other) const noexcept {
  return this->pos > other.pos;
}

template <typename T>
bool Slice<T>::iterator::operator>=(const iterator &other) const noexcept {
  return this->pos >= other.pos;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::begin() const noexcept {
  iterator it;
  it.pos = slicePtr(this);
  it.stride = size_of<T>();
  return it;
}

template <typename T>
typename Slice<T>::iterator Slice<T>::end() const noexcept {
  iterator it = this->begin();
  it.pos = static_cast<char *>(it.pos) + it.stride * this->size();
  return it;
}

template <typename T>
void Slice<T>::swap(Slice &rhs) noexcept {
  std::swap(*this, rhs);
}
#endif // CXXBRIDGE1_RUST_SLICE

#ifndef CXXBRIDGE1_RUST_BITCOPY_T
#define CXXBRIDGE1_RUST_BITCOPY_T
struct unsafe_bitcopy_t final {
  explicit unsafe_bitcopy_t() = default;
};
#endif // CXXBRIDGE1_RUST_BITCOPY_T

#ifndef CXXBRIDGE1_RUST_VEC
#define CXXBRIDGE1_RUST_VEC
template <typename T>
class Vec final {
public:
  using value_type = T;

  Vec() noexcept;
  Vec(std::initializer_list<T>);
  Vec(const Vec &);
  Vec(Vec &&) noexcept;
  ~Vec() noexcept;

  Vec &operator=(Vec &&) & noexcept;
  Vec &operator=(const Vec &) &;

  std::size_t size() const noexcept;
  bool empty() const noexcept;
  const T *data() const noexcept;
  T *data() noexcept;
  std::size_t capacity() const noexcept;

  const T &operator[](std::size_t n) const noexcept;
  const T &at(std::size_t n) const;
  const T &front() const noexcept;
  const T &back() const noexcept;

  T &operator[](std::size_t n) noexcept;
  T &at(std::size_t n);
  T &front() noexcept;
  T &back() noexcept;

  void reserve(std::size_t new_cap);
  void push_back(const T &value);
  void push_back(T &&value);
  template <typename... Args>
  void emplace_back(Args &&...args);
  void truncate(std::size_t len);
  void clear();

  using iterator = typename Slice<T>::iterator;
  iterator begin() noexcept;
  iterator end() noexcept;

  using const_iterator = typename Slice<const T>::iterator;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  void swap(Vec &) noexcept;

  Vec(unsafe_bitcopy_t, const Vec &) noexcept;

private:
  void reserve_total(std::size_t new_cap) noexcept;
  void set_len(std::size_t len) noexcept;
  void drop() noexcept;

  friend void swap(Vec &lhs, Vec &rhs) noexcept { lhs.swap(rhs); }

  std::array<std::uintptr_t, 3> repr;
};

template <typename T>
Vec<T>::Vec(std::initializer_list<T> init) : Vec{} {
  this->reserve_total(init.size());
  std::move(init.begin(), init.end(), std::back_inserter(*this));
}

template <typename T>
Vec<T>::Vec(const Vec &other) : Vec() {
  this->reserve_total(other.size());
  std::copy(other.begin(), other.end(), std::back_inserter(*this));
}

template <typename T>
Vec<T>::Vec(Vec &&other) noexcept : repr(other.repr) {
  new (&other) Vec();
}

template <typename T>
Vec<T>::~Vec() noexcept {
  this->drop();
}

template <typename T>
Vec<T> &Vec<T>::operator=(Vec &&other) & noexcept {
  this->drop();
  this->repr = other.repr;
  new (&other) Vec();
  return *this;
}

template <typename T>
Vec<T> &Vec<T>::operator=(const Vec &other) & {
  if (this != &other) {
    this->drop();
    new (this) Vec(other);
  }
  return *this;
}

template <typename T>
bool Vec<T>::empty() const noexcept {
  return this->size() == 0;
}

template <typename T>
T *Vec<T>::data() noexcept {
  return const_cast<T *>(const_cast<const Vec<T> *>(this)->data());
}

template <typename T>
const T &Vec<T>::operator[](std::size_t n) const noexcept {
  assert(n < this->size());
  auto data = reinterpret_cast<const char *>(this->data());
  return *reinterpret_cast<const T *>(data + n * size_of<T>());
}

template <typename T>
const T &Vec<T>::at(std::size_t n) const {
  if (n >= this->size()) {
    panic<std::out_of_range>("rust::Vec index out of range");
  }
  return (*this)[n];
}

template <typename T>
const T &Vec<T>::front() const noexcept {
  assert(!this->empty());
  return (*this)[0];
}

template <typename T>
const T &Vec<T>::back() const noexcept {
  assert(!this->empty());
  return (*this)[this->size() - 1];
}

template <typename T>
T &Vec<T>::operator[](std::size_t n) noexcept {
  assert(n < this->size());
  auto data = reinterpret_cast<char *>(this->data());
  return *reinterpret_cast<T *>(data + n * size_of<T>());
}

template <typename T>
T &Vec<T>::at(std::size_t n) {
  if (n >= this->size()) {
    panic<std::out_of_range>("rust::Vec index out of range");
  }
  return (*this)[n];
}

template <typename T>
T &Vec<T>::front() noexcept {
  assert(!this->empty());
  return (*this)[0];
}

template <typename T>
T &Vec<T>::back() noexcept {
  assert(!this->empty());
  return (*this)[this->size() - 1];
}

template <typename T>
void Vec<T>::reserve(std::size_t new_cap) {
  this->reserve_total(new_cap);
}

template <typename T>
void Vec<T>::push_back(const T &value) {
  this->emplace_back(value);
}

template <typename T>
void Vec<T>::push_back(T &&value) {
  this->emplace_back(std::move(value));
}

template <typename T>
template <typename... Args>
void Vec<T>::emplace_back(Args &&...args) {
  auto size = this->size();
  this->reserve_total(size + 1);
  ::new (reinterpret_cast<T *>(reinterpret_cast<char *>(this->data()) +
                               size * size_of<T>()))
      T(std::forward<Args>(args)...);
  this->set_len(size + 1);
}

template <typename T>
void Vec<T>::clear() {
  this->truncate(0);
}

template <typename T>
typename Vec<T>::iterator Vec<T>::begin() noexcept {
  return Slice<T>(this->data(), this->size()).begin();
}

template <typename T>
typename Vec<T>::iterator Vec<T>::end() noexcept {
  return Slice<T>(this->data(), this->size()).end();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::begin() const noexcept {
  return this->cbegin();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::end() const noexcept {
  return this->cend();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::cbegin() const noexcept {
  return Slice<const T>(this->data(), this->size()).begin();
}

template <typename T>
typename Vec<T>::const_iterator Vec<T>::cend() const noexcept {
  return Slice<const T>(this->data(), this->size()).end();
}

template <typename T>
void Vec<T>::swap(Vec &rhs) noexcept {
  using std::swap;
  swap(this->repr, rhs.repr);
}

template <typename T>
Vec<T>::Vec(unsafe_bitcopy_t, const Vec &bits) noexcept : repr(bits.repr) {}
#endif // CXXBRIDGE1_RUST_VEC

#ifndef CXXBRIDGE1_IS_COMPLETE
#define CXXBRIDGE1_IS_COMPLETE
namespace detail {
namespace {
template <typename T, typename = std::size_t>
struct is_complete : std::false_type {};
template <typename T>
struct is_complete<T, decltype(sizeof(T))> : std::true_type {};
} // namespace
} // namespace detail
#endif // CXXBRIDGE1_IS_COMPLETE

#ifndef CXXBRIDGE1_LAYOUT
#define CXXBRIDGE1_LAYOUT
class layout {
  template <typename T>
  friend std::size_t size_of();
  template <typename T>
  friend std::size_t align_of();
  template <typename T>
  static typename std::enable_if<std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_size_of() {
    return T::layout::size();
  }
  template <typename T>
  static typename std::enable_if<!std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_size_of() {
    return sizeof(T);
  }
  template <typename T>
  static
      typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
      size_of() {
    return do_size_of<T>();
  }
  template <typename T>
  static typename std::enable_if<std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_align_of() {
    return T::layout::align();
  }
  template <typename T>
  static typename std::enable_if<!std::is_base_of<Opaque, T>::value,
                                 std::size_t>::type
  do_align_of() {
    return alignof(T);
  }
  template <typename T>
  static
      typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
      align_of() {
    return do_align_of<T>();
  }
};

template <typename T>
std::size_t size_of() {
  return layout::size_of<T>();
}

template <typename T>
std::size_t align_of() {
  return layout::align_of<T>();
}
#endif // CXXBRIDGE1_LAYOUT

namespace {
template <bool> struct deleter_if {
  template <typename T> void operator()(T *) {}
};
template <> struct deleter_if<true> {
  template <typename T> void operator()(T *ptr) { ptr->~T(); }
};
} // namespace
} // namespace cxxbridge1
} // namespace rust

#if __cplusplus >= 201402L
#define CXX_DEFAULT_VALUE(value) = value
#else
#define CXX_DEFAULT_VALUE(value)
#endif

namespace vedx64 {
  namespace bridge {
    struct FlowResult;
    struct SemResult;
    using Decoded = ::vedx64::bridge::Decoded;
    using Emu = ::vedx64::bridge::Emu;
    using IrLifted = ::vedx64::bridge::IrLifted;
  }
}

namespace vedx64 {
namespace bridge {
#ifndef CXXBRIDGE1_STRUCT_vedx64$bridge$FlowResult
#define CXXBRIDGE1_STRUCT_vedx64$bridge$FlowResult
struct FlowResult final {
  ::std::uint8_t flow_type CXX_DEFAULT_VALUE(0);
  ::std::uint64_t address CXX_DEFAULT_VALUE(0);
  ::std::uint64_t target CXX_DEFAULT_VALUE(0);
  bool is_indirect CXX_DEFAULT_VALUE(false);
  bool has_fallthrough CXX_DEFAULT_VALUE(false);
  ::std::uint8_t length CXX_DEFAULT_VALUE(0);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_vedx64$bridge$FlowResult

#ifndef CXXBRIDGE1_STRUCT_vedx64$bridge$SemResult
#define CXXBRIDGE1_STRUCT_vedx64$bridge$SemResult
struct SemResult final {
  ::std::uint8_t flags_read CXX_DEFAULT_VALUE(0);
  ::std::uint8_t flags_written CXX_DEFAULT_VALUE(0);
  ::std::uint8_t flags_undefined CXX_DEFAULT_VALUE(0);
  ::std::uint8_t flow CXX_DEFAULT_VALUE(0);
  ::std::uint8_t category CXX_DEFAULT_VALUE(0);
  ::std::uint8_t ring CXX_DEFAULT_VALUE(0);
  bool lock_valid CXX_DEFAULT_VALUE(false);
  bool is_privileged CXX_DEFAULT_VALUE(false);

  using IsRelocatable = ::std::true_type;
};
#endif // CXXBRIDGE1_STRUCT_vedx64$bridge$SemResult

extern "C" {
::vedx64::bridge::Decoded *vedx64$bridge$cxxbridge1$194$decode(::rust::Slice<::std::uint8_t const> code) noexcept {
  ::std::unique_ptr<::vedx64::bridge::Decoded> (*decode$)(::rust::Slice<::std::uint8_t const>) = ::vedx64::bridge::decode;
  return decode$(code).release();
}

void vedx64$bridge$cxxbridge1$194$encode(::vedx64::bridge::Decoded const &d, ::rust::Vec<::std::uint8_t> *return$) noexcept {
  ::rust::Vec<::std::uint8_t> (*encode$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::encode;
  new (return$) ::rust::Vec<::std::uint8_t>(encode$(d));
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$decoded_length(::vedx64::bridge::Decoded const &d) noexcept {
  ::std::uint8_t (*decoded_length$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_length;
  return decoded_length$(d);
}

::std::uint16_t vedx64$bridge$cxxbridge1$194$decoded_mnemonic(::vedx64::bridge::Decoded const &d) noexcept {
  ::std::uint16_t (*decoded_mnemonic$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_mnemonic;
  return decoded_mnemonic$(d);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$decoded_num_operands(::vedx64::bridge::Decoded const &d) noexcept {
  ::std::uint8_t (*decoded_num_operands$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_num_operands;
  return decoded_num_operands$(d);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$decoded_rex(::vedx64::bridge::Decoded const &d) noexcept {
  ::std::uint8_t (*decoded_rex$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_rex;
  return decoded_rex$(d);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$decoded_modrm(::vedx64::bridge::Decoded const &d) noexcept {
  ::std::uint8_t (*decoded_modrm$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_modrm;
  return decoded_modrm$(d);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$decoded_sib(::vedx64::bridge::Decoded const &d) noexcept {
  ::std::uint8_t (*decoded_sib$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_sib;
  return decoded_sib$(d);
}

::std::int64_t vedx64$bridge$cxxbridge1$194$decoded_displacement(::vedx64::bridge::Decoded const &d) noexcept {
  ::std::int64_t (*decoded_displacement$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_displacement;
  return decoded_displacement$(d);
}

::std::int64_t vedx64$bridge$cxxbridge1$194$decoded_immediate(::vedx64::bridge::Decoded const &d) noexcept {
  ::std::int64_t (*decoded_immediate$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_immediate;
  return decoded_immediate$(d);
}

bool vedx64$bridge$cxxbridge1$194$decoded_has_vex(::vedx64::bridge::Decoded const &d) noexcept {
  bool (*decoded_has_vex$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_has_vex;
  return decoded_has_vex$(d);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$decoded_vex_vvvv(::vedx64::bridge::Decoded const &d) noexcept {
  ::std::uint8_t (*decoded_vex_vvvv$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_vex_vvvv;
  return decoded_vex_vvvv$(d);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$decoded_vex_l(::vedx64::bridge::Decoded const &d) noexcept {
  ::std::uint8_t (*decoded_vex_l$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_vex_l;
  return decoded_vex_l$(d);
}

bool vedx64$bridge$cxxbridge1$194$decoded_vex_w(::vedx64::bridge::Decoded const &d) noexcept {
  bool (*decoded_vex_w$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::decoded_vex_w;
  return decoded_vex_w$(d);
}

void vedx64$bridge$cxxbridge1$194$disassemble(::rust::Slice<::std::uint8_t const> code, ::std::uint64_t rip, ::rust::String *return$) noexcept {
  ::rust::String (*disassemble$)(::rust::Slice<::std::uint8_t const>, ::std::uint64_t) = ::vedx64::bridge::disassemble;
  new (return$) ::rust::String(disassemble$(code, rip));
}

void vedx64$bridge$cxxbridge1$194$mnemonic_name(::std::uint16_t m, ::rust::String *return$) noexcept {
  ::rust::String (*mnemonic_name$)(::std::uint16_t) = ::vedx64::bridge::mnemonic_name;
  new (return$) ::rust::String(mnemonic_name$(m));
}

::std::size_t vedx64$bridge$cxxbridge1$194$table_size() noexcept {
  ::std::size_t (*table_size$)() = ::vedx64::bridge::table_size;
  return table_size$();
}

void vedx64$bridge$cxxbridge1$194$assemble(::rust::Str text, ::rust::Vec<::std::uint8_t> *return$) noexcept {
  ::rust::Vec<::std::uint8_t> (*assemble$)(::rust::Str) = ::vedx64::bridge::assemble;
  new (return$) ::rust::Vec<::std::uint8_t>(assemble$(text));
}

void vedx64$bridge$cxxbridge1$194$assemble_block(::rust::Str text, ::rust::Vec<::std::uint8_t> *return$) noexcept {
  ::rust::Vec<::std::uint8_t> (*assemble_block$)(::rust::Str) = ::vedx64::bridge::assemble_block;
  new (return$) ::rust::Vec<::std::uint8_t>(assemble_block$(text));
}

bool vedx64$bridge$cxxbridge1$194$can_relocate(::rust::Slice<::std::uint8_t const> code) noexcept {
  bool (*can_relocate$)(::rust::Slice<::std::uint8_t const>) = ::vedx64::bridge::can_relocate;
  return can_relocate$(code);
}

bool vedx64$bridge$cxxbridge1$194$is_rip_relative(::rust::Slice<::std::uint8_t const> code) noexcept {
  bool (*is_rip_relative$)(::rust::Slice<::std::uint8_t const>) = ::vedx64::bridge::is_rip_relative;
  return is_rip_relative$(code);
}

void vedx64$bridge$cxxbridge1$194$classify_flow(::rust::Slice<::std::uint8_t const> code, ::std::uint64_t addr, ::vedx64::bridge::FlowResult *return$) noexcept {
  ::vedx64::bridge::FlowResult (*classify_flow$)(::rust::Slice<::std::uint8_t const>, ::std::uint64_t) = ::vedx64::bridge::classify_flow;
  new (return$) ::vedx64::bridge::FlowResult(classify_flow$(code, addr));
}

void vedx64$bridge$cxxbridge1$194$get_semantics(::vedx64::bridge::Decoded const &d, ::vedx64::bridge::SemResult *return$) noexcept {
  ::vedx64::bridge::SemResult (*get_semantics$)(::vedx64::bridge::Decoded const &) = ::vedx64::bridge::get_semantics;
  new (return$) ::vedx64::bridge::SemResult(get_semantics$(d));
}

bool vedx64$bridge$cxxbridge1$194$is_jcc(::std::uint16_t m) noexcept {
  bool (*is_jcc$)(::std::uint16_t) = ::vedx64::bridge::is_jcc;
  return is_jcc$(m);
}

bool vedx64$bridge$cxxbridge1$194$is_cmov(::std::uint16_t m) noexcept {
  bool (*is_cmov$)(::std::uint16_t) = ::vedx64::bridge::is_cmov;
  return is_cmov$(m);
}

bool vedx64$bridge$cxxbridge1$194$is_call(::std::uint16_t m) noexcept {
  bool (*is_call$)(::std::uint16_t) = ::vedx64::bridge::is_call;
  return is_call$(m);
}

bool vedx64$bridge$cxxbridge1$194$is_ret(::std::uint16_t m) noexcept {
  bool (*is_ret$)(::std::uint16_t) = ::vedx64::bridge::is_ret;
  return is_ret$(m);
}

bool vedx64$bridge$cxxbridge1$194$is_unconditional_branch(::std::uint16_t m) noexcept {
  bool (*is_unconditional_branch$)(::std::uint16_t) = ::vedx64::bridge::is_unconditional_branch;
  return is_unconditional_branch$(m);
}

bool vedx64$bridge$cxxbridge1$194$is_relative_branch(::std::uint16_t m) noexcept {
  bool (*is_relative_branch$)(::std::uint16_t) = ::vedx64::bridge::is_relative_branch;
  return is_relative_branch$(m);
}

bool vedx64$bridge$cxxbridge1$194$changes_rip(::std::uint16_t m) noexcept {
  bool (*changes_rip$)(::std::uint16_t) = ::vedx64::bridge::changes_rip;
  return changes_rip$(m);
}

bool vedx64$bridge$cxxbridge1$194$is_arith(::std::uint16_t m) noexcept {
  bool (*is_arith$)(::std::uint16_t) = ::vedx64::bridge::is_arith;
  return is_arith$(m);
}

bool vedx64$bridge$cxxbridge1$194$is_logical(::std::uint16_t m) noexcept {
  bool (*is_logical$)(::std::uint16_t) = ::vedx64::bridge::is_logical;
  return is_logical$(m);
}

bool vedx64$bridge$cxxbridge1$194$is_shift(::std::uint16_t m) noexcept {
  bool (*is_shift$)(::std::uint16_t) = ::vedx64::bridge::is_shift;
  return is_shift$(m);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$jcc_condition(::std::uint16_t m) noexcept {
  ::std::uint8_t (*jcc_condition$)(::std::uint16_t) = ::vedx64::bridge::jcc_condition;
  return jcc_condition$(m);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$cmov_condition(::std::uint16_t m) noexcept {
  ::std::uint8_t (*cmov_condition$)(::std::uint16_t) = ::vedx64::bridge::cmov_condition;
  return cmov_condition$(m);
}

::std::uint16_t vedx64$bridge$cxxbridge1$194$jcc_for_condition(::std::uint8_t cc) noexcept {
  ::std::uint16_t (*jcc_for_condition$)(::std::uint8_t) = ::vedx64::bridge::jcc_for_condition;
  return jcc_for_condition$(cc);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$sets_eflags(::std::uint16_t m) noexcept {
  ::std::uint8_t (*sets_eflags$)(::std::uint16_t) = ::vedx64::bridge::sets_eflags;
  return sets_eflags$(m);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$reads_eflags(::std::uint16_t m) noexcept {
  ::std::uint8_t (*reads_eflags$)(::std::uint16_t) = ::vedx64::bridge::reads_eflags;
  return reads_eflags$(m);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$canonical_size(::std::uint16_t m) noexcept {
  ::std::uint8_t (*canonical_size$)(::std::uint16_t) = ::vedx64::bridge::canonical_size;
  return canonical_size$(m);
}

void vedx64$bridge$cxxbridge1$194$build_jmp_rel32(::std::int32_t disp, ::rust::Vec<::std::uint8_t> *return$) noexcept {
  ::rust::Vec<::std::uint8_t> (*build_jmp_rel32$)(::std::int32_t) = ::vedx64::bridge::build_jmp_rel32;
  new (return$) ::rust::Vec<::std::uint8_t>(build_jmp_rel32$(disp));
}

void vedx64$bridge$cxxbridge1$194$build_jcc_rel32(::std::uint8_t cc, ::std::int32_t disp, ::rust::Vec<::std::uint8_t> *return$) noexcept {
  ::rust::Vec<::std::uint8_t> (*build_jcc_rel32$)(::std::uint8_t, ::std::int32_t) = ::vedx64::bridge::build_jcc_rel32;
  new (return$) ::rust::Vec<::std::uint8_t>(build_jcc_rel32$(cc, disp));
}

void vedx64$bridge$cxxbridge1$194$build_call_rel32(::std::int32_t disp, ::rust::Vec<::std::uint8_t> *return$) noexcept {
  ::rust::Vec<::std::uint8_t> (*build_call_rel32$)(::std::int32_t) = ::vedx64::bridge::build_call_rel32;
  new (return$) ::rust::Vec<::std::uint8_t>(build_call_rel32$(disp));
}

void vedx64$bridge$cxxbridge1$194$build_mov_imm64(::std::uint8_t reg_id, ::std::uint64_t imm, ::rust::Vec<::std::uint8_t> *return$) noexcept {
  ::rust::Vec<::std::uint8_t> (*build_mov_imm64$)(::std::uint8_t, ::std::uint64_t) = ::vedx64::bridge::build_mov_imm64;
  new (return$) ::rust::Vec<::std::uint8_t>(build_mov_imm64$(reg_id, imm));
}

::vedx64::bridge::Emu *vedx64$bridge$cxxbridge1$194$emu_new(::std::size_t mem_size) noexcept {
  ::std::unique_ptr<::vedx64::bridge::Emu> (*emu_new$)(::std::size_t) = ::vedx64::bridge::emu_new;
  return emu_new$(mem_size).release();
}

::std::int32_t vedx64$bridge$cxxbridge1$194$emu_step(::vedx64::bridge::Emu &e) noexcept {
  ::std::int32_t (*emu_step$)(::vedx64::bridge::Emu &) = ::vedx64::bridge::emu_step;
  return emu_step$(e);
}

::std::size_t vedx64$bridge$cxxbridge1$194$emu_run(::vedx64::bridge::Emu &e, ::std::size_t max_steps) noexcept {
  ::std::size_t (*emu_run$)(::vedx64::bridge::Emu &, ::std::size_t) = ::vedx64::bridge::emu_run;
  return emu_run$(e, max_steps);
}

::std::uint64_t vedx64$bridge$cxxbridge1$194$emu_rip(::vedx64::bridge::Emu const &e) noexcept {
  ::std::uint64_t (*emu_rip$)(::vedx64::bridge::Emu const &) = ::vedx64::bridge::emu_rip;
  return emu_rip$(e);
}

void vedx64$bridge$cxxbridge1$194$emu_set_rip(::vedx64::bridge::Emu &e, ::std::uint64_t v) noexcept {
  void (*emu_set_rip$)(::vedx64::bridge::Emu &, ::std::uint64_t) = ::vedx64::bridge::emu_set_rip;
  emu_set_rip$(e, v);
}

::std::uint64_t vedx64$bridge$cxxbridge1$194$emu_gpr(::vedx64::bridge::Emu const &e, ::std::size_t i) noexcept {
  ::std::uint64_t (*emu_gpr$)(::vedx64::bridge::Emu const &, ::std::size_t) = ::vedx64::bridge::emu_gpr;
  return emu_gpr$(e, i);
}

void vedx64$bridge$cxxbridge1$194$emu_set_gpr(::vedx64::bridge::Emu &e, ::std::size_t i, ::std::uint64_t v) noexcept {
  void (*emu_set_gpr$)(::vedx64::bridge::Emu &, ::std::size_t, ::std::uint64_t) = ::vedx64::bridge::emu_set_gpr;
  emu_set_gpr$(e, i, v);
}

::std::uint64_t vedx64$bridge$cxxbridge1$194$emu_rflags(::vedx64::bridge::Emu const &e) noexcept {
  ::std::uint64_t (*emu_rflags$)(::vedx64::bridge::Emu const &) = ::vedx64::bridge::emu_rflags;
  return emu_rflags$(e);
}

void vedx64$bridge$cxxbridge1$194$emu_set_rflags(::vedx64::bridge::Emu &e, ::std::uint64_t v) noexcept {
  void (*emu_set_rflags$)(::vedx64::bridge::Emu &, ::std::uint64_t) = ::vedx64::bridge::emu_set_rflags;
  emu_set_rflags$(e, v);
}

void vedx64$bridge$cxxbridge1$194$emu_write_mem(::vedx64::bridge::Emu &e, ::std::size_t offset, ::rust::Slice<::std::uint8_t const> data) noexcept {
  void (*emu_write_mem$)(::vedx64::bridge::Emu &, ::std::size_t, ::rust::Slice<::std::uint8_t const>) = ::vedx64::bridge::emu_write_mem;
  emu_write_mem$(e, offset, data);
}

void vedx64$bridge$cxxbridge1$194$emu_read_mem(::vedx64::bridge::Emu const &e, ::std::size_t offset, ::std::size_t len, ::rust::Vec<::std::uint8_t> *return$) noexcept {
  ::rust::Vec<::std::uint8_t> (*emu_read_mem$)(::vedx64::bridge::Emu const &, ::std::size_t, ::std::size_t) = ::vedx64::bridge::emu_read_mem;
  new (return$) ::rust::Vec<::std::uint8_t>(emu_read_mem$(e, offset, len));
}

::vedx64::bridge::IrLifted *vedx64$bridge$cxxbridge1$194$ir_lift(::rust::Slice<::std::uint8_t const> code, ::std::uint64_t addr) noexcept {
  ::std::unique_ptr<::vedx64::bridge::IrLifted> (*ir_lift$)(::rust::Slice<::std::uint8_t const>, ::std::uint64_t) = ::vedx64::bridge::ir_lift;
  return ir_lift$(code, addr).release();
}

::std::uint64_t vedx64$bridge$cxxbridge1$194$ir_lifted_address(::vedx64::bridge::IrLifted const &l) noexcept {
  ::std::uint64_t (*ir_lifted_address$)(::vedx64::bridge::IrLifted const &) = ::vedx64::bridge::ir_lifted_address;
  return ir_lifted_address$(l);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$ir_lifted_length(::vedx64::bridge::IrLifted const &l) noexcept {
  ::std::uint8_t (*ir_lifted_length$)(::vedx64::bridge::IrLifted const &) = ::vedx64::bridge::ir_lifted_length;
  return ir_lifted_length$(l);
}

::std::size_t vedx64$bridge$cxxbridge1$194$ir_lifted_op_count(::vedx64::bridge::IrLifted const &l) noexcept {
  ::std::size_t (*ir_lifted_op_count$)(::vedx64::bridge::IrLifted const &) = ::vedx64::bridge::ir_lifted_op_count;
  return ir_lifted_op_count$(l);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$ir_lifted_op_opcode(::vedx64::bridge::IrLifted const &l, ::std::size_t i) noexcept {
  ::std::uint8_t (*ir_lifted_op_opcode$)(::vedx64::bridge::IrLifted const &, ::std::size_t) = ::vedx64::bridge::ir_lifted_op_opcode;
  return ir_lifted_op_opcode$(l, i);
}

::std::uint8_t vedx64$bridge$cxxbridge1$194$ir_lifted_op_num_inputs(::vedx64::bridge::IrLifted const &l, ::std::size_t i) noexcept {
  ::std::uint8_t (*ir_lifted_op_num_inputs$)(::vedx64::bridge::IrLifted const &, ::std::size_t) = ::vedx64::bridge::ir_lifted_op_num_inputs;
  return ir_lifted_op_num_inputs$(l, i);
}
} // extern "C"
} // namespace bridge
} // namespace vedx64

extern "C" {
static_assert(::rust::detail::is_complete<::std::remove_extent<::vedx64::bridge::Decoded>::type>::value, "definition of `::vedx64::bridge::Decoded` is required");
static_assert(sizeof(::std::unique_ptr<::vedx64::bridge::Decoded>) == sizeof(void *), "");
static_assert(alignof(::std::unique_ptr<::vedx64::bridge::Decoded>) == alignof(void *), "");
void cxxbridge1$unique_ptr$vedx64$bridge$Decoded$null(::std::unique_ptr<::vedx64::bridge::Decoded> *ptr) noexcept {
  ::new (ptr) ::std::unique_ptr<::vedx64::bridge::Decoded>();
}
void cxxbridge1$unique_ptr$vedx64$bridge$Decoded$raw(::std::unique_ptr<::vedx64::bridge::Decoded> *ptr, ::std::unique_ptr<::vedx64::bridge::Decoded>::pointer raw) noexcept {
  ::new (ptr) ::std::unique_ptr<::vedx64::bridge::Decoded>(raw);
}
::std::unique_ptr<::vedx64::bridge::Decoded>::element_type const *cxxbridge1$unique_ptr$vedx64$bridge$Decoded$get(::std::unique_ptr<::vedx64::bridge::Decoded> const &ptr) noexcept {
  return ptr.get();
}
::std::unique_ptr<::vedx64::bridge::Decoded>::pointer cxxbridge1$unique_ptr$vedx64$bridge$Decoded$release(::std::unique_ptr<::vedx64::bridge::Decoded> &ptr) noexcept {
  return ptr.release();
}
void cxxbridge1$unique_ptr$vedx64$bridge$Decoded$drop(::std::unique_ptr<::vedx64::bridge::Decoded> *ptr) noexcept {
  ::rust::deleter_if<::rust::detail::is_complete<::vedx64::bridge::Decoded>::value>{}(ptr);
}

static_assert(::rust::detail::is_complete<::std::remove_extent<::vedx64::bridge::Emu>::type>::value, "definition of `::vedx64::bridge::Emu` is required");
static_assert(sizeof(::std::unique_ptr<::vedx64::bridge::Emu>) == sizeof(void *), "");
static_assert(alignof(::std::unique_ptr<::vedx64::bridge::Emu>) == alignof(void *), "");
void cxxbridge1$unique_ptr$vedx64$bridge$Emu$null(::std::unique_ptr<::vedx64::bridge::Emu> *ptr) noexcept {
  ::new (ptr) ::std::unique_ptr<::vedx64::bridge::Emu>();
}
void cxxbridge1$unique_ptr$vedx64$bridge$Emu$raw(::std::unique_ptr<::vedx64::bridge::Emu> *ptr, ::std::unique_ptr<::vedx64::bridge::Emu>::pointer raw) noexcept {
  ::new (ptr) ::std::unique_ptr<::vedx64::bridge::Emu>(raw);
}
::std::unique_ptr<::vedx64::bridge::Emu>::element_type const *cxxbridge1$unique_ptr$vedx64$bridge$Emu$get(::std::unique_ptr<::vedx64::bridge::Emu> const &ptr) noexcept {
  return ptr.get();
}
::std::unique_ptr<::vedx64::bridge::Emu>::pointer cxxbridge1$unique_ptr$vedx64$bridge$Emu$release(::std::unique_ptr<::vedx64::bridge::Emu> &ptr) noexcept {
  return ptr.release();
}
void cxxbridge1$unique_ptr$vedx64$bridge$Emu$drop(::std::unique_ptr<::vedx64::bridge::Emu> *ptr) noexcept {
  ::rust::deleter_if<::rust::detail::is_complete<::vedx64::bridge::Emu>::value>{}(ptr);
}

static_assert(::rust::detail::is_complete<::std::remove_extent<::vedx64::bridge::IrLifted>::type>::value, "definition of `::vedx64::bridge::IrLifted` is required");
static_assert(sizeof(::std::unique_ptr<::vedx64::bridge::IrLifted>) == sizeof(void *), "");
static_assert(alignof(::std::unique_ptr<::vedx64::bridge::IrLifted>) == alignof(void *), "");
void cxxbridge1$unique_ptr$vedx64$bridge$IrLifted$null(::std::unique_ptr<::vedx64::bridge::IrLifted> *ptr) noexcept {
  ::new (ptr) ::std::unique_ptr<::vedx64::bridge::IrLifted>();
}
void cxxbridge1$unique_ptr$vedx64$bridge$IrLifted$raw(::std::unique_ptr<::vedx64::bridge::IrLifted> *ptr, ::std::unique_ptr<::vedx64::bridge::IrLifted>::pointer raw) noexcept {
  ::new (ptr) ::std::unique_ptr<::vedx64::bridge::IrLifted>(raw);
}
::std::unique_ptr<::vedx64::bridge::IrLifted>::element_type const *cxxbridge1$unique_ptr$vedx64$bridge$IrLifted$get(::std::unique_ptr<::vedx64::bridge::IrLifted> const &ptr) noexcept {
  return ptr.get();
}
::std::unique_ptr<::vedx64::bridge::IrLifted>::pointer cxxbridge1$unique_ptr$vedx64$bridge$IrLifted$release(::std::unique_ptr<::vedx64::bridge::IrLifted> &ptr) noexcept {
  return ptr.release();
}
void cxxbridge1$unique_ptr$vedx64$bridge$IrLifted$drop(::std::unique_ptr<::vedx64::bridge::IrLifted> *ptr) noexcept {
  ::rust::deleter_if<::rust::detail::is_complete<::vedx64::bridge::IrLifted>::value>{}(ptr);
}
} // extern "C"
