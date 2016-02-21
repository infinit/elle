// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file
 * \brief Contains an implementation of C++17 optional (n3793).
 */

#ifndef NETWORK_URI_OPTIONAL_INC
#define NETWORK_URI_OPTIONAL_INC

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <memory>
#include <algorithm>

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
#ifdef NDEBUG
#define NETWORK_ASSERTED_EXPRESSION(CHECK, EXPR) (EXPR)
#else
#define NETWORK_ASSERTED_EXPRESSION(CHECK, EXPR) \
  ((CHECK) ? (EXPR) : (fail(#CHECK, __FILE__, __LINE__), (EXPR)))
inline void fail(const char* expr, const char* file, unsigned line) {}
#endif  // NDEBUG
#endif // !defined(DOXYGEN_SHOULD_SKIP_THIS)

namespace network {
struct nullopt_t {
  struct init {};
  constexpr nullopt_t(init) {}
};
const nullopt_t nullopt{nullopt_t::init{}};

class bad_optional_access : public std::logic_error {
 public:
  explicit bad_optional_access(const std::string& what_arg)
      : std::logic_error(what_arg) {}

  explicit bad_optional_access(const char* what_arg)
      : std::logic_error(what_arg) {}
};

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
namespace details {
struct dummy_t {};

template <class T>
union trivially_destructible_optional_storage {
  static_assert(std::is_trivially_destructible<T>::value, "");

  dummy_t dummy_;
  T value_;

  constexpr trivially_destructible_optional_storage() : dummy_{} {}

  constexpr trivially_destructible_optional_storage(const T& v) : value_{v} {}

  ~trivially_destructible_optional_storage() = default;
};

template <class T>
union optional_storage {
  dummy_t dummy_;
  T value_;

  constexpr optional_storage() : dummy_{} {}

  constexpr optional_storage(const T& v) : value_{v} {}

  ~optional_storage() {}
};

template <class T>
class trivially_destructible_optional_base {
public:
  typedef T value_type;

  constexpr trivially_destructible_optional_base() noexcept
      : init_(false),
        storage_{} {}

  constexpr trivially_destructible_optional_base(const T& v)
      : init_(true), storage_{v} {}

  constexpr trivially_destructible_optional_base(T&& v)
      : init_(true), storage_{std::move(v)} {}

  ~trivially_destructible_optional_base() = default;

protected:

  bool init_;
  optional_storage<T> storage_;
};

template <class T>
class optional_base {
public:
  typedef T value_type;

  constexpr optional_base() noexcept
      : init_(false),
        storage_{} {}

  constexpr optional_base(const T& v)
      : init_(true), storage_{v} {}

  constexpr optional_base(T&& v)
      : init_(true), storage_{std::move(v)} {}

  ~optional_base() {
    if (init_) {
      storage_.value_.T::~T();
    }
  }

protected:

  bool init_;
  optional_storage<T> storage_;
};
} // namespace details
#endif // !defined(DOXYGEN_SHOULD_SKIP_THIS)

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
template <class T>
using optional_base = typename std::conditional<
  std::is_trivially_destructible<T>::value,
  details::trivially_destructible_optional_base<T>,
  details::optional_base<T>>::type;
#endif // !defined(DOXYGEN_SHOULD_SKIP_THIS)

template <class T>
class optional : optional_base<T> {
  typedef optional_base<T> base_type;

 public:
  typedef T value_type;

  constexpr optional() : optional_base<T>() {}

  constexpr optional(nullopt_t) noexcept : optional_base<T>() {}

  optional(const optional& other) {
    if (other) {
      ::new(static_cast<void*>(ptr())) T(*other);
      base_type::init_ = true;
    }
  }

  optional(optional&& other) noexcept {
    if (other) {
      ::new(static_cast<void*>(ptr())) T(std::move(other.storage_.value_));
      base_type::init_ = true;
    }
  }

  constexpr optional(const T& v) : optional_base<T>(v) {}

  constexpr optional(T&& v) : optional_base<T>(std::move(v)) {}

  optional& operator=(nullopt_t) noexcept {
    ptr()->T::~T();
    base_type::init_ = false;
    return *this;
  }

  optional& operator=(const optional& other) {
    if (bool(*this)  && !other) {
      ptr()->T::~T();
      base_type::init_ = false;
    }
    else if (!(*this) && bool(other)) {
      ::new(static_cast<void*>(ptr())) T(*other);
      base_type::init_ = true;
    }
    else if (bool(*this) && bool(other)) {
      base_type::storage_.value_ = *other;
    }
    return *this;
  }

  optional& operator=(optional&& other) noexcept {
    if (bool(*this)  && !other) {
      ptr()->T::~T();
      base_type::init_ = false;
    }
    else if (!(*this) && bool(other)) {
      ::new(static_cast<void*>(ptr())) T(std::move(*other));
      base_type::init_ = true;
    }
    else if (bool(*this) && bool(other)) {
      base_type::storage_.value_ = std::move(*other);
    }
    return *this;
  }

  ~optional() = default;

  void swap(optional& other) noexcept {
    if (bool(*this)  && !other) {
      ::new(static_cast<void*>(other.ptr())) T(std::move(**this));
      ptr()->T::~T();
      std::swap(base_type::init_, other.base_type::init_);
    }
    else if (!(*this) && bool(other)) {
      ::new(static_cast<void*>(ptr())) T(std::move(*other));
      other.ptr()->T::~T();
      std::swap(base_type::init_, other.init_);
    }
    else if (bool(*this) && bool(other)) {
      std::swap(**this, *other);
    }
  }

  constexpr T const* operator->() const {
    return NETWORK_ASSERTED_EXPRESSION(bool(*this), ptr());
  }

  T* operator->() {
    return NETWORK_ASSERTED_EXPRESSION(bool(*this), ptr());
  }

  constexpr T const& operator*() const {
    return NETWORK_ASSERTED_EXPRESSION(bool(*this), base_type::storage_.value_);
  }

  T& operator*() {
    return NETWORK_ASSERTED_EXPRESSION(bool(*this), base_type::storage_.value_);
  }

  constexpr explicit operator bool() const noexcept { return base_type::init_; }

  constexpr T const& value() const {
    return *this ? base_type::storage_.value_
                 : (throw bad_optional_access("Uninitialized optional value"),
                    base_type::storage_.value_);
  }

  T& value() {
    return *this ? base_type::storage_.value_
                 : (throw bad_optional_access("Uninitialized optional value"),
                    base_type::storage_.value_);
  }

  template <class U>
  T value_or(U&& other) const & {
    if (!base_type::init_) {
      return std::forward(other);
    }
    return *(*this);
  }

  template <class U>
  T value_or(U&& other) && {
    if (!base_type::init_) {
      return std::forward(other);
    }
    return std::move(*(*this));
  }

 private:

  T* ptr() {
    return std::addressof(base_type::storage_.value_);
  }

};

template <class T>
bool operator==(const optional<T>& lhs, const optional<T>& rhs) {
  if (bool(lhs) != bool(rhs)) {
    return false;
  } else if (!bool(lhs)) {
    return true;
  } else {
    return *lhs == *rhs;
  }
}

template <class T>
bool operator!=(const optional<T>& lhs, const optional<T>& rhs) {
  return !(lhs == rhs);
}

template <class T>
bool operator<(const optional<T>& lhs, const optional<T>& rhs) {
  if (!rhs) {
    return false;
  } else if (!lhs) {
    return true;
  } else {
    return *lhs < *rhs;
  }
}

template <class T>
bool operator>(const optional<T>& lhs, const optional<T>& rhs) {
  return rhs < lhs;
}

template <class T>
bool operator<=(const optional<T>& lhs, const optional<T>& rhs) {
  return !(rhs < lhs);
}

template <class T>
bool operator>=(const optional<T>& lhs, const optional<T>& rhs) {
  return !(lhs < rhs);
}

template <class T>
bool operator==(const optional<T>& x, nullopt_t) noexcept {
  return !x;
}

template <class T>
bool operator==(nullopt_t, const optional<T>& x) noexcept {
  return !x;
}

template <class T>
bool operator!=(const optional<T>& x, nullopt_t) noexcept {
  return bool(x);
}

template <class T>
bool operator!=(nullopt_t, const optional<T>& x) noexcept {
  return bool(x);
}

template <class T>
bool operator<(const optional<T>& x, nullopt_t) noexcept {
  return false;
}

template <class T>
bool operator<(nullopt_t, const optional<T>& x) noexcept {
  return bool(x);
}

template <class T>
bool operator<=(const optional<T>& x, nullopt_t) noexcept {
  return !x;
}

template <class T>
bool operator<=(nullopt_t, const optional<T>& x) noexcept {
  return true;
}

template <class T>
bool operator>(const optional<T>& x, nullopt_t) noexcept {
  return bool(x);
}

template <class T>
bool operator>(nullopt_t, const optional<T>& x) noexcept {
  return false;
}

template <class T>
bool operator>=(const optional<T>& x, nullopt_t) noexcept {
  return true;
}

template <class T>
bool operator>=(nullopt_t, const optional<T>& x) noexcept {
  return !x;
}

template <class T>
bool operator==(const optional<T>& x, const T& v) {
  return bool(x) ? *x == v : false;
}

template <class T>
bool operator==(const T& v, const optional<T>& x) {
  return bool(x) ? v == *x : false;
}

template <class T>
bool operator!=(const optional<T>& x, const T& v) {
  return bool(x) ? !(*x == v) : true;
}

template <class T>
bool operator!=(const T& v, const optional<T>& x) {
  return bool(x) ? !(v == *x) : true;
}

template <class T>
bool operator<(const optional<T>& x, const T& v) {
  return bool(x) ? *x < v : true;
}

template <class T>
bool operator<(const T& v, const optional<T>& x) {
  return bool(x) ? v < *x : false;
}

template <class T>
bool operator>(const optional<T>& x, const T& v) {
  return bool(x) ? *x < v : true;
}

template <class T>
bool operator>(const T& v, const optional<T>& x) {
  return bool(x) ? v < *x : false;
}

template <class T>
bool operator>=(const optional<T>& x, const T& v) {
  return !(x < v);
}

template <class T>
bool operator>=(const T& v, const optional<T>& x) {
  return !(v < x);
}

template <class T>
bool operator<=(const optional<T>& x, const T& v) {
  return !(x > v);
}

template <class T>
bool operator<=(const T& v, const optional<T>& x) {
  return !(v > x);
}

template <class T>
void swap(optional<T>& lhs,
          optional<T>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  return lhs.swap(rhs);
}

template <class T>
constexpr optional<typename std::decay<T>::type> make_optional(T&& v) {
  return optional<typename std::decay<T>::type>(std::forward(v));
}
}  // namespace network

#endif  // NETWORK_URI_OPTIONAL_INC
