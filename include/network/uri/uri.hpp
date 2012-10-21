// Copyright 2009-2010 Jeroen Habraken.
// Copyright 2009-2012 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_URI_INC
#define NETWORK_URI_INC

#include <network/uri/config.hpp>
#include <network/uri/detail/uri_parts.hpp>
#include <network/uri/schemes.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/functional/hash_fwd.hpp>
#include <algorithm>
#include <functional>


namespace network {
  namespace detail {
#if defined(BOOST_WINDOWS)
    bool parse(std::wstring::const_iterator first,
               std::wstring::const_iterator last,
               uri_parts<std::wstring::const_iterator> &parts);
#else
    bool parse(std::string::const_iterator first,
               std::string::const_iterator last,
               uri_parts<std::string::const_iterator> &parts);
#endif // defined(BOOST_WINDOWS)

    template <class T>
    inline
    void hash_combine(std::size_t& seed, const T& v) {
      std::hash<T> hasher;
      seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
  } // namespace detail

  class uri {

    friend class builder;

  public:

#if defined(BOOST_WINDOWS)
    typedef wchar_t value_type;
#else
    typedef char value_type;
#endif // defined(BOOST_WINDOWS)

    typedef std::basic_string<value_type> string_type;
    typedef string_type::const_iterator iterator;
    typedef string_type::const_iterator const_iterator;

    class part_range {

    public:

#if defined(BOOST_WINDOWS)
      typedef wchar_t value_type;
#else
      typedef char value_type;
#endif // defined(BOOST_WINDOWS)

      typedef std::basic_string<value_type> string_type;
      typedef string_type::const_iterator iterator;
      typedef string_type::const_iterator const_iterator;

      typedef bool (part_range::*unspecified_bool_type)() const;

      part_range() {

      }

      part_range(const_iterator first, const_iterator last)
        : first_(first), last_(last) {

      }

      const_iterator begin() const {
        return first_;
      }

      const_iterator end() const {
        return last_;
      }

      bool empty() const {
        return (first_ == last_);
      }

      operator unspecified_bool_type () const {
        return !empty()? &part_range::empty : nullptr;
      }

      string_type native() const {
        return string_type(first_, last_);
      }

      std::string string() const {
        return std::string(first_, last_);
      }

      std::wstring wstring() const {
        return std::wstring(first_, last_);
      }

    private:

      const_iterator first_, last_;

    };

    uri()
      : is_valid_(false) {

    }

    uri(const std::string &uri)
      : uri_(std::begin(uri), std::end(uri)), is_valid_(false) {
      parse();
    }

    uri(const std::wstring &uri)
      : uri_(std::begin(uri), std::end(uri)), is_valid_(false) {
      parse();
    }

    template <
      class FwdIter
      >
    uri(const FwdIter &first, const FwdIter &last)
      : uri_(first, last), is_valid_(false) {
      parse();
    }

    uri(const uri &other)
      : uri_(other.uri_) {
      parse();
    }

    uri &operator = (const uri &other) {
      uri_ = other.uri_;
      parse();
      return *this;
    }

    uri &operator = (const std::string &uri) {
      uri_ = string_type(std::begin(uri), std::end(uri));
      parse();
      return *this;
    }

    uri &operator = (const std::wstring &uri) {
      uri_ = string_type(std::begin(uri), std::end(uri));
      parse();
      return *this;
    }

    ~uri() {

    }

    void swap(uri &other) {
      std::swap(uri_, other.uri_);
      std::swap(uri_parts_, other.uri_parts_);
      std::swap(is_valid_, other.is_valid_);
    }

    const_iterator begin() const {
      return uri_.begin();
    }

    const_iterator end() const {
      return uri_.end();
    }

    part_range scheme() const {
      return part_range(std::begin(uri_parts_.scheme), std::end(uri_parts_.scheme));
    }

    part_range user_info() const {
      return uri_parts_.hier_part.user_info?
        part_range(std::begin(uri_parts_.hier_part.user_info.get()),
		   std::end(uri_parts_.hier_part.user_info.get()))
        : part_range();
    }

    part_range host() const {
      return uri_parts_.hier_part.host?
        part_range(std::begin(uri_parts_.hier_part.host.get()),
                   std::end(uri_parts_.hier_part.host.get()))
        : part_range();
    }

    part_range port() const {
      return uri_parts_.hier_part.port?
        part_range(std::begin(uri_parts_.hier_part.port.get()),
                   std::end(uri_parts_.hier_part.port.get()))
        : part_range();
    }

    part_range path() const {
      return uri_parts_.hier_part.path?
        part_range(std::begin(uri_parts_.hier_part.path.get()),
                   std::end(uri_parts_.hier_part.path.get()))
        : part_range();
    }

    part_range query() const {
      return uri_parts_.query ?
        part_range(std::begin(uri_parts_.query.get()),
		   std::end(uri_parts_.query.get()))
        : part_range();
    }

    part_range fragment() const {
      return uri_parts_.fragment?
        part_range(std::begin(uri_parts_.fragment.get()),
		   std::end(uri_parts_.fragment.get()))
        : part_range();
    }

    part_range authority() const {
      return part_range(user_info().begin(), port().end());
    }

    string_type native() const {
      return uri_;
    }

    const value_type *c_str() const {
      return uri_.c_str();
    }

    std::string string() const {
      return std::string(std::begin(uri_), std::end(uri_));
    }

    std::wstring wstring() const {
      return std::wstring(std::begin(uri_), std::end(uri_));
    }

    bool empty() const {
      return uri_.empty();
    }

    bool is_valid() const {
      return is_valid_;
    }

    bool is_absolute() const {
      return is_valid() && !boost::empty(scheme());
    }

    bool is_opaque() const {
      return is_absolute();
    }

    void append(const string_type &data) {
      uri_.append(data);
      parse();
    }

    template <
      class FwdIter
      >
    void append(const FwdIter &first, const FwdIter &last) {
      uri_.append(first, last);
      parse();
    }

  private:

    void parse();

    string_type uri_;
    detail::uri_parts<const_iterator> uri_parts_;
    bool is_valid_;

  };

  inline
  void uri::parse() {
    const_iterator first(std::begin(uri_)), last(std::end(uri_));
    is_valid_ = detail::parse(first, last, uri_parts_);
    if (is_valid_) {
      if (!uri_parts_.scheme) {
        uri_parts_.scheme = part_range(std::begin(uri_),
                                       std::begin(uri_));
      }
      uri_parts_.update();
    }
  }

  inline
  void swap(uri &lhs, uri &rhs) {
    lhs.swap(rhs);
  }

  inline
  std::size_t hash_value(const uri &uri_)
  {
    std::size_t seed = 0;
    std::for_each(std::begin(uri_), std::end(uri_),
                  [&seed] (uri::value_type c) { detail::hash_combine(seed, c); });
    return seed;
  }

  bool operator == (const uri &lhs, const uri &rhs);

  inline
  bool operator == (const uri &lhs, const uri::string_type &rhs) {
    return lhs == uri(rhs);
  }

  inline
  bool operator == (const uri::string_type &lhs, const uri &rhs) {
    return uri(lhs) == rhs;
  }

  inline
  bool operator == (const uri &lhs, const uri::value_type *rhs) {
    return lhs == uri(rhs);
  }

  inline
  bool operator == (const uri::value_type *lhs, const uri &rhs) {
    return uri(lhs) == rhs;
  }

  inline
  bool operator != (const uri &lhs, const uri &rhs) {
    return !(lhs == rhs);
  }

  inline
  bool operator < (const uri &lhs, const uri &rhs) {
    return lhs.string() < rhs.string();
  }
} // namespace network


#endif // NETWORK_URI_INC
