// Copyright 2009-2010 Jeroen Habraken.
// Copyright 2009-2012 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_URI_INC
#define NETWORK_URI_INC

#include <network/uri/detail/uri_parts.hpp>
#include <iterator>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <system_error>
#include <cstring>


namespace network {
  namespace detail {
    bool parse(std::string::const_iterator first,
               std::string::const_iterator last,
               uri_parts<std::string::const_iterator> &parts);

    template <class T>
    inline
    void hash_combine(std::size_t& seed, const T& v) {
      std::hash<T> hasher;
      seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    inline
    std::string translate(const std::string &source) {
      return source;
    }

    inline
    std::string translate(const std::wstring &source) {
      return std::string(std::begin(source), std::end(source));
    }

  } // namespace detail

  class uri_syntax_error : public std::runtime_error {

  public:

    uri_syntax_error(const char *what) : std::runtime_error(what) {

    }

    virtual ~uri_syntax_error() {

    }

  };

  class uri {

    friend class uri_builder;

  public:

    typedef std::string string_type;
    typedef string_type::const_iterator iterator;
    typedef string_type::const_iterator const_iterator;
    typedef std::iterator_traits<const_iterator>::value_type value_type;

    class part_range {

    public:

      typedef std::string string_type;
      typedef string_type::const_iterator iterator;
      typedef string_type::const_iterator const_iterator;
      typedef std::iterator_traits<const_iterator>::value_type value_type;

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

      std::u16string u16string() const {
	return std::u16string(first_, last_);
      }

      std::u32string u32string() const {
	return std::u32string(first_, last_);
      }

    private:

      const_iterator first_, last_;

    };

    uri() {

    }

    template <
      class InputIter
      >
    uri(const InputIter &first, const InputIter &last)
      : uri_(first, last) {
      parse();
    }

    template <
      class Source
      >
    uri(const Source &uri)
      : uri_(detail::translate(uri)) {
      parse();
    }

    uri(const uri &other)
      : uri_(other.uri_) {
      parse();
    }

    ~uri() {

    }

    uri &operator = (const uri &other) {
      uri_ = other.uri_;
      parse();
      return *this;
    }

    void swap(uri &other) {
      std::swap(uri_, other.uri_);
      std::swap(uri_parts_, other.uri_parts_);
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

    std::string string() const {
      return std::string(std::begin(uri_), std::end(uri_));
    }

    std::wstring wstring() const {
      return std::wstring(std::begin(uri_), std::end(uri_));
    }

    std::u16string u16string() const {
      return std::u16string(std::begin(uri_), std::end(uri_));
    }

    std::u32string u32string() const {
      return std::u32string(std::begin(uri_), std::end(uri_));
    }

    bool empty() const {
      return uri_.empty();
    }

    bool absolute() const {
      return !scheme().empty();
    }

    bool opaque() const {
      return absolute();
    }

    uri normalize() const;

    uri relativize(const uri &other) const;

    uri resolve(const uri &other) const;

  private:

    void parse();

    string_type uri_;
    detail::uri_parts<const_iterator> uri_parts_;

  };

  inline
  void uri::parse() {
    if (uri_.empty()) {
      return;
    }

    const_iterator first(std::begin(uri_)), last(std::end(uri_));
    bool is_valid = detail::parse(first, last, uri_parts_);
    if (!is_valid) {
      throw uri_syntax_error("Unable to parse URI string.");
    }

    if (!uri_parts_.scheme) {
      uri_parts_.scheme = part_range(std::begin(uri_),
				     std::begin(uri_));
    }
    uri_parts_.update();
  }

  template <
    class Source
    >
  inline
  uri make_uri(const Source &source, std::error_code &ec) {
    return uri();
  }

  inline
  void swap(uri &lhs, uri &rhs) {
    lhs.swap(rhs);
  }

  inline
  std::size_t hash_value(const uri &uri_) {
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
  bool operator != (const uri &lhs, const uri &rhs) {
    return !(lhs == rhs);
  }

  inline
  bool operator < (const uri &lhs, const uri &rhs) {
    return lhs.native() < rhs.native();
  }

  inline
  bool operator > (const uri &lhs, const uri &rhs) {
    return rhs < lhs;
  }

  inline
  bool operator <= (const uri &lhs, const uri &rhs) {
    return !(rhs < lhs);
  }

  inline
  bool operator >= (const uri &lhs, const uri &rhs) {
    return !(lhs < rhs);
  }
} // namespace network


#endif // NETWORK_URI_INC
