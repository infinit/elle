// Copyright 2009-2010 Jeroen Habraken.
// Copyright 2009-2013 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_URI_INC
#define NETWORK_URI_INC

#include <network/uri/config.hpp>
#include <network/uri/detail/translate.hpp>
#include <network/utility/string_ref.hpp>
#include <boost/optional.hpp>
#include <iterator>
#include <system_error>
#include <algorithm>
#include <functional>


namespace network {
  enum class uri_error {
    // parser errors
    invalid_syntax = 1,

    // builder errors
    invalid_uri,
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

    uri_category_impl() NETWORK_URI_DEFAULTED_FUNCTION;

    virtual ~uri_category_impl() NETWORK_URI_NOEXCEPT;

    virtual const char *name() const NETWORK_URI_NOEXCEPT;

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
    typedef const_iterator iterator;
    typedef std::iterator_traits<iterator>::value_type value_type;

  private:

    uri(boost::optional<string_type> scheme,
	boost::optional<string_type> user_info,
	boost::optional<string_type> host,
	boost::optional<string_type> port,
	boost::optional<string_type> path,
	boost::optional<string_type> query,
	boost::optional<string_type> fragment);

  public:

    uri();

    template <
      class InputIter
      >
    uri(const InputIter &first, const InputIter &last) {
      std::error_code ec;
      initialize(string_type(first, last), ec);
      if (ec) {
	throw std::system_error(ec);
      }
    }

    template <
      class Source
      >
    explicit uri(const Source &uri) {
      std::error_code ec;
      initialize(detail::translate(uri), ec);
      if (ec) {
	throw std::system_error(ec);
      }
    }

    template <
      class Source
      >
    explicit uri(const Source &uri, std::error_code &ec) {
      initialize(detail::translate(uri), ec);
    }

    uri(const uri &other);

    uri(uri &&other);

    ~uri();

    uri &operator = (const uri &other);

    uri &operator = (uri &&other);

    void swap(uri &other); // noexcept

    const_iterator begin() const;

    const_iterator end() const;

    boost::optional<string_ref> scheme() const;

    boost::optional<string_ref> user_info() const;

    boost::optional<string_ref> host() const;

    boost::optional<string_ref> port() const;

    boost::optional<string_ref> path() const;

    boost::optional<string_ref> query() const;

    boost::optional<string_ref> fragment() const;

    boost::optional<string_ref> authority() const;

    string_type native() const;

    std::string string() const;

    std::wstring wstring() const;

    std::u16string u16string() const;

    std::u32string u32string() const;

    bool empty() const; // noexcept

    bool absolute() const; // noexcept

    bool opaque() const; // noexcept

    uri normalize(uri_comparison_level level) const;

    int compare(const uri &other, uri_comparison_level level) const;

    uri relativize(const uri &other, uri_comparison_level level) const;

    uri resolve(const uri &other, uri_comparison_level level) const;

  private:

    void initialize(const string_type &uri, std::error_code &ec);

    struct impl;
    impl *pimpl_;

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

  inline
  bool operator == (const uri &lhs, const uri &rhs) {
    return lhs.compare(rhs, uri_comparison_level::path_segment_normalization) == 0;
  }

  inline
  bool operator != (const uri &lhs, const uri &rhs) {
    return !(lhs == rhs);
  }

  inline
  bool operator < (const uri &lhs, const uri &rhs) {
    return lhs.compare(rhs, uri_comparison_level::path_segment_normalization) < 0;
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
