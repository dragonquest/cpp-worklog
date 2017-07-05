#ifndef ATL_OPTIONAL_H_
#define ATL_OPTIONAL_H_

#include <type_traits>

namespace atl {

template <typename T>
class Optional {
 public:
  constexpr Optional() = default;

  Optional(const T& value) : used_(true) {
    new(&value_) T(value);
  }

  Optional(T& value) : used_(true) {
    new(&value_) T(value);
  }

  Optional(T&& value) : used_(true) {
    new(&value_) T(value);
  }

  ~Optional() {
    reset();
  }

  constexpr explicit operator bool() const {
    return used_;
  }

  constexpr bool has_value() const {
    return used_;
  }

  void reset() {
    used_ = false;
    if(!std::is_trivially_destructible<T>::value) {
      static_cast<T*>(static_cast<void*>(&value_))->~T();
    }
  }

  // constexpr const T* operator->() const {
  //   return std::addressof(pointer());
  // }
  constexpr const T* operator->() const {
    return pointer();
  }

  // constexpr T& operator*() const& {
  //   return *pointer();
  // }

  constexpr const T& operator*() const& {
    return *pointer();
  }

  T& value() & {
    return *pointer();
  }

  constexpr T& value() const& {
    return *pointer();
  }

  constexpr T&& value() const&& {
    return std::move(*value_);
  }

  template<typename U>
  T value_or(U&& default_value) const& {
    if(has_value()) {
      return **this;
    }
    return static_cast<T>(std::forward<U>(default_value));
  }

  // http://en.cppreference.com/w/cpp/utility/optional/swap
  void swap(Optional& other) {
    using std::swap;

    swap(used_, other.used_);
    swap(**this, *other);
  }

 private:
  const T* pointer() const { return static_cast<const T*>(static_cast<const void*>(&value_)); }
  T* pointer() { return static_cast<T*>(static_cast<void*>(&value_)); }

  typename std::aligned_storage<sizeof(T), alignof(T)>::type value_;
  bool used_ = false;  // It's called engaged (as in "scharf") in std::optional
};

template <typename T>
constexpr Optional<std::decay<T>> make_optional( T&& value ) {
  return Optional<T>(std::forward<T>(value));
}

} // namespace atl

#endif  // ATL_OPTIONAL_H_
