// Copyright 2009-2010 Jeroen Habraken.
// Copyright 2009-2013 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_URI_INC
#define NETWORK_URI_INC

#include <network/uri/config.hpp>
#include <network/uri/uri_errors.hpp>
#include <network/uri/detail/encode.hpp>
#include <network/uri/detail/decode.hpp>
#include <network/uri/detail/translate.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/optional.hpp>
#include <iterator>
#include <algorithm>
#include <functional>


namespace network {
  namespace detail {
    template <
      class FwdIter
      >
    struct hierarchical_part {
      boost::optional<boost::iterator_range<FwdIter> > user_info;
      boost::optional<boost::iterator_range<FwdIter> > host;
      boost::optional<boost::iterator_range<FwdIter> > port;
      boost::optional<boost::iterator_range<FwdIter> > path;
    };

    template <
      class FwdIter
      >
    struct uri_parts {
      boost::optional<boost::iterator_range<FwdIter> > scheme;
      hierarchical_part<FwdIter> hier_part;
      boost::optional<boost::iterator_range<FwdIter> > query;
      boost::optional<boost::iterator_range<FwdIter> > fragment;
    };
  } // namespace detail

  enum class uri_comparison_level {
    string_comparison,
    case_normalization,
    percent_encoding_normalization,
    path_segment_normalization,
    scheme_normalization,
    protocol_normalization
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

    void initialize(boost::optional<string_type> scheme,
		    boost::optional<string_type> user_info,
		    boost::optional<string_type> host,
		    boost::optional<string_type> port,
		    boost::optional<string_type> path,
		    boost::optional<string_type> query,
		    boost::optional<string_type> fragment);

  public:

    uri();

    template <class InputIter>
    uri(InputIter first, InputIter last) {
      if (!initialize(string_type(first, last))) {
	throw uri_syntax_error();
      }
    }

    template <class InputIter>
    explicit uri(InputIter first, InputIter last, std::error_code &ec) {
      if (!initialize(string_type(first, last))) {
	ec = make_error_code(uri_error::invalid_syntax);
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

    uri(uri &&other) NETWORK_URI_NOEXCEPT;

    ~uri() NETWORK_URI_NOEXCEPT;

    uri &operator = (uri other);

    void swap(uri &other) NETWORK_URI_NOEXCEPT;

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
#if !defined(_MSC_VER)
    template <typename CharT, class CharTraits = std::char_traits<CharT>, class Alloc = std::allocator<CharT> >
    std::basic_string<CharT, CharTraits, Alloc> string(const Alloc &alloc = Alloc()) const {
      return std::basic_string<CharT, CharTraits, Alloc>(begin(), end());
    }
#else
    template <typename CharT, class CharTraits, class Alloc>
    std::basic_string<CharT, CharTraits, Alloc> string(const Alloc &alloc = Alloc()) const {
      return std::basic_string<CharT, CharTraits, Alloc>(begin(), end());
    }
#endif // !/defined(_MSC_VER)
    std::string string() const;
    std::wstring wstring() const;
    std::u16string u16string() const;
    std::u32string u32string() const;

    bool empty() const NETWORK_URI_NOEXCEPT;
    bool is_absolute() const NETWORK_URI_NOEXCEPT;
    bool is_opaque() const NETWORK_URI_NOEXCEPT;
    bool absolute() const NETWORK_URI_NOEXCEPT { return is_absolute(); }
    bool opaque() const NETWORK_URI_NOEXCEPT { return is_opaque(); }

    uri normalize(uri_comparison_level level) const;
    uri make_reference(const uri &other, uri_comparison_level level) const;
    uri resolve(const uri &other, uri_comparison_level level) const;

    int compare(const uri &other, uri_comparison_level level) const NETWORK_URI_NOEXCEPT;

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

    string_type uri_;
    detail::uri_parts<string_type::iterator> uri_parts_;

  };

  template <class Source>
  inline
  uri make_uri(const Source &source, std::error_code &ec) NETWORK_URI_NOEXCEPT {
    return uri(source, ec);
  }

  template <class InputIter>
  inline
  uri make_uri(InputIter first, InputIter last, std::error_code &ec) NETWORK_URI_NOEXCEPT {
    return uri(first, last, ec);
  }

  void swap(uri &lhs, uri &rhs) NETWORK_URI_NOEXCEPT;

  bool operator == (const uri &lhs, const uri &rhs);

  bool operator == (const uri &lhs, const char *rhs);

  inline
  bool operator == (const char *lhs, const uri &rhs) {
    return rhs == lhs;
  }

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
