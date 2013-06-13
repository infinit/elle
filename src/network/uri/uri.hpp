// Copyright 2009-2010 Jeroen Habraken.
// Copyright 2009-2013 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_URI_INC
#define NETWORK_URI_INC

#include <network/uri/config.hpp>
#include <network/uri/detail/encode.hpp>
#include <network/uri/detail/decode.hpp>
#include <network/uri/detail/translate.hpp>
#include <boost/utility/string_ref.hpp>
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

    uri_category_impl() = default;

    virtual ~uri_category_impl() noexcept;

    virtual const char *name() const noexcept;

    virtual std::string message(int ev) const;

  };

  inline
  const std::error_category &uri_category() {
    static uri_category_impl uri_category;
    return uri_category;
  }

  inline
  std::error_code make_error_code(uri_error e) {
    return std::error_code(static_cast<int>(e), uri_category());
  }

  class uri_syntax_error : public std::system_error {

  public:

    uri_syntax_error()
      : std::system_error(make_error_code(uri_error::invalid_syntax)) {

    }

    virtual ~uri_syntax_error() noexcept {

    }

  };

  class uri_builder_error : public std::system_error {

  public:

    uri_builder_error()
      : std::system_error(make_error_code(uri_error::invalid_uri)) {

    }

    virtual ~uri_builder_error() noexcept {

    }

  };

  enum class uri_comparison_level {
    string_comparison,
    case_normalization,
    percent_encoding_normalization,
    path_segment_normalization,
  };

  class uri_builder;

  class NETWORK_URI_DECL uri {

    friend class uri_builder;

  public:

    typedef std::string string_type;
    typedef string_type::const_iterator const_iterator;
    typedef const_iterator iterator;
    typedef std::iterator_traits<iterator>::value_type value_type;
    typedef boost::string_ref string_view;

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

    template <class InputIter>
    uri(const InputIter &first, const InputIter &last) {
      if (!initialize(string_type(first, last))) {
	throw uri_syntax_error();
      }
    }

    template <class Source>
    explicit uri(const Source &uri) {
      if (!initialize(detail::translate(uri))) {
	throw uri_syntax_error();
      }
    }

    template <class Source>
    explicit uri(const Source &uri, std::error_code &ec) {
      if (!initialize(detail::translate(uri))) {
	ec = make_error_code(uri_error::invalid_syntax);
      }
    }

    explicit uri(const uri_builder &builder);

    uri(const uri &other);

    uri(uri &&other);

    ~uri();

    uri &operator = (uri other);

    void swap(uri &other) noexcept;

    const_iterator begin() const;
    const_iterator end() const;

    boost::optional<string_view> scheme() const;
    boost::optional<string_view> user_info() const;
    boost::optional<string_view> host() const;
    boost::optional<string_view> port() const;
    boost::optional<string_view> path() const;
    boost::optional<string_view> query() const;
    boost::optional<string_view> fragment() const;
    boost::optional<string_view> authority() const;

    string_type native() const;
    template <typename CharT, class CharTraits = std::char_traits<CharT>, class Alloc = std::allocator<CharT> >
    std::basic_string<CharT, CharTraits, Alloc> string(const Alloc &alloc = Alloc()) const {
      return std::basic_string<CharT, CharTraits, Alloc>(begin(), end());
    }
    std::string string() const;
    std::wstring wstring() const;
    std::u16string u16string() const;
    std::u32string u32string() const;

    bool empty() const noexcept;
    bool is_absolute() const noexcept;
    bool is_opaque() const noexcept;
    bool absolute() const noexcept { return is_absolute(); }
    bool opaque() const noexcept { return is_opaque(); }

    uri normalize(uri_comparison_level level) const;
    uri make_reference(const uri &other, uri_comparison_level level) const;
    uri resolve(const uri &other, uri_comparison_level level) const;

    int compare(const uri &other, uri_comparison_level level) const;

    template <typename InputIter, typename OutputIter>
    static OutputIter encode_user_info(InputIter first, InputIter last, OutputIter out) {
      return detail::encode_user_info(first, last, out);
    }

    template <typename InputIter, typename OutputIter>
    static OutputIter encode_host(InputIter first, InputIter last, OutputIter out) {
      return detail::encode_host(first, last, out);
    }

    template <typename InputIter, typename OutputIter>
    static OutputIter encode_port(InputIter first, InputIter last, OutputIter out) {
      return detail::encode_port(first, last, out);
    }

    template <typename InputIter, typename OutputIter>
    static OutputIter encode_path(InputIter first, InputIter last, OutputIter out) {
      return detail::encode_path(first, last, out);
    }

    template <typename InputIter, typename OutputIter>
    static OutputIter encode_query(InputIter first, InputIter last, OutputIter out) {
      return detail::encode_query(first, last, out);
    }

    template <typename InputIter, typename OutputIter>
    static OutputIter encode_fragment(InputIter first, InputIter last, OutputIter out) {
      return detail::encode_fragment(first, last, out);
    }

    template <typename InputIter, typename OutputIter>
    static OutputIter decode(InputIter first, InputIter last, OutputIter out) {
      return detail::decode(first, last, out);
    }

  private:

    bool initialize(const string_type &uri);

    struct impl;
    impl *pimpl_;

  };

  template <class Source>
  inline
  uri make_uri(const Source &source, std::error_code &ec) noexcept {
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
  bool operator == (const uri &lhs, const char *rhs) {
    if (std::strlen(rhs) != std::size_t(std::distance(std::begin(lhs), std::end(lhs)))) {
      return false;
    }
    return std::equal(std::begin(lhs), std::end(lhs), rhs);
  }

  inline
  bool operator == (const char *lhs, const uri &rhs) {
    return rhs == lhs;
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
