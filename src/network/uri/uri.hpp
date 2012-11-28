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
#include <exception>
#include <cstring>


namespace network {
  namespace detail {
    bool parse(std::string::const_iterator first,
               std::string::const_iterator last,
               uri_parts<std::string::const_iterator> &parts);

    inline
    std::string translate(const std::string &source) {
      return source;
    }

    inline
    std::string translate(const std::wstring &source) {
      return std::string(std::begin(source), std::end(source));
    }

  } // namespace detail

  enum class uri_error {
    syntax_error = 1,
  };

  class uri_category_impl : public std::error_category {

  public:

    virtual const char *name() const;
    virtual std::string message(int ev) const;

  };

  const std::error_category &uri_category();

  std::error_code make_error_code(uri_error e);

  class uri_syntax_error : public std::exception {

  public:

    uri_syntax_error();
    virtual ~uri_syntax_error();
    virtual const char *what() const;

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

      part_range();

      part_range(const_iterator first, const_iterator last);

      const_iterator begin() const;

      const_iterator end() const;

      bool empty() const;

      operator unspecified_bool_type () const;

      string_type native() const;

      std::string string() const;

      std::wstring wstring() const;

      std::u16string u16string() const;

      std::u32string u32string() const;

    private:

      const_iterator first_, last_;

    };

    uri();

    template <
      class InputIter
      >
    uri(const InputIter &first, const InputIter &last)
      : uri_(first, last) {
      std::error_code ec;
      parse(ec);
      if (ec) {
	throw uri_syntax_error();
      }
    }

    template <
      class Source
      >
    explicit uri(const Source &uri)
      : uri_(detail::translate(uri)) {
      std::error_code ec;
      parse(ec);
      if (ec) {
	throw uri_syntax_error();
      }
    }

    template <
      class Source
      >
    explicit uri(const Source &uri, std::error_code &ec)
      : uri_(detail::translate(uri)) {
      parse(ec);
    }

    uri(const uri &other);

    ~uri();

    uri &operator = (const uri &other);

    void swap(uri &other);

    const_iterator begin() const;

    const_iterator end() const;

    part_range scheme() const;

    part_range user_info() const;

    part_range host() const;

    part_range port() const;

    part_range path() const;

    part_range query() const;

    part_range fragment() const;

    part_range authority() const;

    string_type native() const;

    std::string string() const;

    std::wstring wstring() const;

    std::u16string u16string() const;

    std::u32string u32string() const;

    bool empty() const;

    bool absolute() const;

    bool opaque() const;

    uri normalize() const;

    uri relativize(const uri &other) const;

    uri resolve(const uri &other) const;

  private:

    void parse(std::error_code &ec);

    string_type uri_;
    detail::uri_parts<const_iterator> uri_parts_;

  };

  template <
    class Source
    >
  inline
  uri make_uri(const Source &source, std::error_code &ec) { // noexcept {
    return uri(source, ec);
  }

  void swap(uri &lhs, uri &rhs);

  std::size_t hash_value(const uri &uri_);

  enum uri_comparison_level {
    string_comparison,
    case_normalization,
    percent_encoding_normalization,
    path_segment_normalization,
  };

  bool equals(const uri &lhs, const uri &rhs, uri_comparison_level level);

  bool operator == (const uri &lhs, const uri &rhs);

  inline
  bool operator != (const uri &lhs, const uri &rhs) {
    return !(lhs == rhs);
  }

  bool operator < (const uri &lhs, const uri &rhs);

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

namespace std {
  template <>
  struct is_error_code_enum<network::uri_error> : public true_type { };
} // namespace std

namespace std {
  template <>
  struct hash<network::uri> {

    std::size_t operator () (const network::uri &uri_) const {
      return network::hash_value(uri_);
    }

  };
} // namespace std

#include <network/uri/uri_builder.hpp>


#endif // NETWORK_URI_INC
