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
#include <network/uri/detail/translate.hpp>
#include <boost/optional.hpp>
#include <iterator>
#include <exception>
#include <system_error>
#include <algorithm>
#include <functional>


namespace network {
  enum class uri_error {
    // parser errors
    invalid_syntax = 1,

    // builder errors
    invalid_scheme,
    invalid_user_info,
    invalid_host,
    invalid_port,
    invalid_path,
    invalid_query,
    invalid_fragment,
  };

  class NETWORK_URI_DECL uri_category_impl : public std::error_category {

  public:

#if defined(BOOST_NO_CXX11_NOEXCEPT)
    virtual const char *name() const;
#else
    virtual const char *name() const noexcept;
#endif // defined(BOOST_NO_CXX11_NOEXCEPT)
    virtual std::string message(int ev) const;

  };

  NETWORK_URI_DECL const std::error_category &uri_category();

  NETWORK_URI_DECL std::error_code make_error_code(uri_error e);

  enum class uri_comparison_level {
    string_comparison,
    case_normalization,
    percent_encoding_normalization,
    path_segment_normalization,
  };

  class NETWORK_URI_DECL uri {

    friend class uri_builder;

  public:

    typedef std::string string_type;
    typedef string_type::const_iterator const_iterator;
    typedef string_type::iterator iterator;
    typedef std::iterator_traits<iterator>::value_type value_type;

    class part_range {

    public:

      typedef uri::string_type string_type;
      typedef uri::const_iterator const_iterator;
      typedef uri::iterator iterator;
      typedef uri::value_type value_type;

      typedef bool (part_range::*unspecified_bool_type)() const;

      part_range();

      part_range(const_iterator first, const_iterator last);

      const_iterator begin() const;

      const_iterator end() const;

      bool empty() const;

      operator unspecified_bool_type () const;

      string_type native() const;

      std::string string() const;

    private:

      const_iterator first_, last_;

    };

  private:

    uri(const boost::optional<string_type> &scheme,
	const boost::optional<string_type> &user_info,
	const boost::optional<string_type> &host,
	const boost::optional<string_type> &port,
	const boost::optional<string_type> &path,
	const boost::optional<string_type> &query,
	const boost::optional<string_type> &fragment);

  public:

    uri();

    template <
      class InputIter
      >
    uri(const InputIter &first, const InputIter &last)
      : uri_(first, last) {
      std::error_code ec;
      parse(ec);
      if (ec) {
	throw std::system_error(ec);
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
	throw std::system_error(ec);
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

    boost::optional<part_range> scheme() const;

    boost::optional<part_range> user_info() const;

    boost::optional<part_range> host() const;

    boost::optional<part_range> port() const;

    boost::optional<part_range> path() const;

    boost::optional<part_range> query() const;

    boost::optional<part_range> fragment() const;

    boost::optional<part_range> authority() const;

    string_type native() const;

    std::string string() const;

    std::wstring wstring() const;

    std::u16string u16string() const;

    std::u32string u32string() const;

    bool empty() const;

    bool absolute() const;

    bool opaque() const;

    uri normalize(uri_comparison_level comparison_level) const;

    uri relativize(const uri &other) const;

    uri resolve(const uri &other) const;

  private:

    void parse(std::error_code &ec);

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif // defined(_MSC_VER)

    string_type uri_;

#if defined(_MSC_VER)
#pragma warning(push)
#endif // defined(_MSC_VER)

    detail::uri_parts<const_iterator> uri_parts_;

  };

  template <
    class Source
    >
  inline
  uri make_uri(const Source &source, std::error_code &ec) { // noexcept {
    return uri(source, ec);
  }

  inline
  void swap(uri &lhs, uri &rhs) {
    lhs.swap(rhs);
  }

  NETWORK_URI_DECL bool equals(const uri &lhs, const uri &rhs, uri_comparison_level level);

  inline
  bool operator == (const uri &lhs, const uri &rhs) {
    return equals(lhs, rhs, uri_comparison_level::path_segment_normalization);
  }

  inline
  bool operator != (const uri &lhs, const uri &rhs) {
    return !(lhs == rhs);
  }

  inline
  bool operator < (const uri &lhs, const uri &rhs) {
    return
      lhs.normalize(uri_comparison_level::path_segment_normalization).native() <
      rhs.normalize(uri_comparison_level::path_segment_normalization).native();
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

namespace std {
  template <>
  struct is_error_code_enum<network::uri_error> : public true_type { };
} // namespace std

namespace std {
  template <>
  struct hash<network::uri> {

    std::size_t operator () (const network::uri &uri_) const {
      std::size_t seed = 0;
      std::for_each(std::begin(uri_), std::end(uri_),
		    [&seed] (network::uri::value_type v) {
		      std::hash<network::uri::value_type> hasher;
		      seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
		    });
      return seed;
    }

  };
} // namespace std

#include <network/uri/uri_builder.hpp>


#endif // NETWORK_URI_INC
