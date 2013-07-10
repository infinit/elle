// Copyright 2012, 2013 Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <network/uri/uri.hpp>
#include "detail/uri_parse.hpp"
#include "detail/uri_percent_encode.hpp"
#include "detail/uri_normalize.hpp"
#include "detail/uri_resolve.hpp"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <cassert>
#include <cctype>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>

namespace network {
  uri_category_impl::~uri_category_impl() NETWORK_URI_NOEXCEPT {

  }

  const char *uri_category_impl::name() const NETWORK_URI_NOEXCEPT {
    static const char name[] = "uri_error";
    return name;
  }

  std::string uri_category_impl::message(int ev) const {
    switch (uri_error(ev)) {
    case uri_error::invalid_syntax:
      return "Unable to parse URI string.";
    default:
      break;
    }
    return "Unknown URI error.";
  }

  const std::error_category &uri_category() {
    static uri_category_impl uri_category;
    return uri_category;
  }

  std::error_code make_error_code(uri_error e) {
    return std::error_code(static_cast<int>(e), uri_category());
  }

  uri_syntax_error::uri_syntax_error()
    : std::system_error(make_error_code(uri_error::invalid_syntax)) {

  }

  uri_syntax_error::~uri_syntax_error() NETWORK_URI_NOEXCEPT {

  }

  uri_builder_error::uri_builder_error()
    : std::system_error(make_error_code(uri_error::invalid_uri)) {

  }

  uri_builder_error::~uri_builder_error() NETWORK_URI_NOEXCEPT {

  }

  namespace {
    inline
    boost::iterator_range<uri::string_type::iterator>
    copy_range(uri::string_type::iterator first,
	       uri::string_type::iterator last,
	       uri::string_type::iterator &it) {
      auto part_first = it;
      std::advance(it, std::distance(first, last));
      return boost::make_iterator_range(part_first, it);
    }

    void advance_parts(uri::string_type::iterator first,
		       uri::string_type::iterator last,
		       detail::uri_parts<uri::string_type::iterator> &parts,
		       const detail::uri_parts<uri::string_type::iterator> &existing_parts) {
      auto it = first;
      if (auto scheme = existing_parts.scheme) {
	parts.scheme.reset(copy_range(std::begin(*scheme), std::end(*scheme), it));

	// ignore : for all URIs
	if (':' == *it) {
	  ++it;
	}

	// ignore // for hierarchical URIs
	if (existing_parts.hier_part.host) {
	  while ('/' == *it) {
	    ++it;
	  }
	}
      }

      if (auto user_info = existing_parts.hier_part.user_info) {
	parts.hier_part.user_info.reset(copy_range(std::begin(*user_info), std::end(*user_info), it));
	++it; // ignore @
      }

      if (auto host = existing_parts.hier_part.host) {
	parts.hier_part.host.reset(copy_range(std::begin(*host), std::end(*host), it));
      }

      if (auto port = existing_parts.hier_part.port) {
	++it; // ignore :
	parts.hier_part.port.reset(copy_range(std::begin(*port), std::end(*port), it));
      }

      if (auto path = existing_parts.hier_part.path) {
	parts.hier_part.path.reset(copy_range(std::begin(*path), std::end(*path), it));
      }

      if (auto query = existing_parts.query) {
	++it; // ignore ?
	parts.query.reset(copy_range(std::begin(*query), std::end(*query), it));
      }

      if (auto fragment = existing_parts.fragment) {
	++it; // ignore #
	parts.fragment.reset(copy_range(std::begin(*fragment), std::end(*fragment), it));
      }
    }

    void advance_parts(boost::iterator_range<uri::string_type::iterator> range,
		       detail::uri_parts<uri::string_type::iterator> &parts,
		       const detail::uri_parts<uri::string_type::iterator> &existing_parts) {
      advance_parts(std::begin(range), std::end(range), parts, existing_parts);
    }
  } // namespace

  struct uri::impl {

    string_type uri_;
    detail::uri_parts<string_type::iterator> uri_parts_;

    impl *clone();

  };

  uri::impl *uri::impl::clone() {
    std::unique_ptr<impl> other(new (std::nothrow) impl);
    assert(other.get() && "Unable to allocate memory for network::uri");
    other->uri_ = uri_;
    advance_parts(boost::as_literal(other->uri_), other->uri_parts_, uri_parts_);
    return other.release();
  }

  void uri::initialize(boost::optional<string_type> scheme,
		       boost::optional<string_type> user_info,
		       boost::optional<string_type> host,
		       boost::optional<string_type> port,
		       boost::optional<string_type> path,
		       boost::optional<string_type> query,
		       boost::optional<string_type> fragment) {
    if (scheme) {
      pimpl_->uri_.append(*scheme);
    }

    if (user_info || host || port) {
      if (scheme) {
	pimpl_->uri_.append("://");
      }

      if (user_info) {
	pimpl_->uri_.append(*user_info);
	pimpl_->uri_.append("@");
      }

      if (host) {
	pimpl_->uri_.append(*host);
      }
      else {
	throw uri_builder_error();
      }

      if (port) {
	pimpl_->uri_.append(":");
	pimpl_->uri_.append(*port);
      }
    }
    else {
      if (scheme) {
	if (path || query || fragment) {
	  pimpl_->uri_.append(":");
	}
	else {
	  throw uri_builder_error();
	}
      }
    }

    if (path) {
      pimpl_->uri_.append(*path);
    }

    if (query) {
      pimpl_->uri_.append("?");
      pimpl_->uri_.append(*query);
    }

    if (fragment) {
      pimpl_->uri_.append("#");
      pimpl_->uri_.append(*fragment);
    }

    auto it = std::begin(pimpl_->uri_);
    if (scheme) {
      pimpl_->uri_parts_.scheme.reset(copy_range(std::begin(*scheme), std::end(*scheme), it));
      // ignore ://
      while ((':' == *it) || ('/' == *it)) {
	++it;
      }
    }

    if (user_info) {
      pimpl_->uri_parts_.hier_part.user_info.reset(copy_range(std::begin(*user_info), std::end(*user_info), it));
      ++it; // ignore @
    }

    if (host) {
      pimpl_->uri_parts_.hier_part.host.reset(copy_range(std::begin(*host), std::end(*host), it));
    }

    if (port) {
      ++it; // ignore :
      pimpl_->uri_parts_.hier_part.port.reset(copy_range(std::begin(*port), std::end(*port), it));
    }

    if (path) {
      pimpl_->uri_parts_.hier_part.path.reset(copy_range(std::begin(*path), std::end(*path), it));
    }

    if (query) {
      ++it; // ignore ?
      pimpl_->uri_parts_.query.reset(copy_range(std::begin(*query), std::end(*query), it));
    }

    if (fragment) {
      ++it; // ignore #
      pimpl_->uri_parts_.fragment.reset(copy_range(std::begin(*fragment), std::end(*fragment), it));
    }
  }

  uri::uri()
    : pimpl_(new (std::nothrow) impl) {
    assert(pimpl_ && "Unable to allocate memory for network::uri");
  }

  uri::uri(const uri &other)
    : pimpl_(other.pimpl_->clone()) {
    assert(pimpl_ && "Unable to allocate memory for network::uri");
  }

  uri::uri(const uri_builder &builder)
    : pimpl_(new (std::nothrow) impl) {
    assert(pimpl_ && "Unable to allocate memory for network::uri");
    initialize(builder.scheme_,
	       builder.user_info_,
	       builder.host_,
	       builder.port_,
	       builder.path_,
	       builder.query_,
	       builder.fragment_);
  }

  uri::uri(uri &&other)
    : pimpl_(new (std::nothrow) impl) {
    assert(other.pimpl_ && "Unable to allocate memory for network::uri");
    std::swap(pimpl_, other.pimpl_);
  }

  uri::~uri() {
    delete pimpl_;
  }

  uri &uri::operator = (uri other) {
    other.swap(*this);
    return *this;
  }

  void uri::swap(uri &other) NETWORK_URI_NOEXCEPT {
    std::swap(pimpl_, other.pimpl_);
  }

  uri::const_iterator uri::begin() const {
    return pimpl_->uri_.begin();
  }

  uri::const_iterator uri::end() const {
    return pimpl_->uri_.end();
  }

  namespace
  {
    inline boost::string_ref to_string_ref(const uri::string_type &uri,
					   boost::iterator_range<uri::const_iterator> uri_part) {
      if (!uri_part.empty()) {
	const char *c_str = uri.c_str();
	const char *uri_part_begin = &(*(std::begin(uri_part)));
	std::advance(c_str, std::distance(c_str, uri_part_begin));
	return boost::string_ref(c_str, std::distance(std::begin(uri_part), std::end(uri_part)));
      }
      return boost::string_ref();
    }

    inline boost::string_ref to_string_ref(boost::string_ref::const_iterator uri_part_begin,
					   boost::string_ref::const_iterator uri_part_end)
    {
      return boost::string_ref(uri_part_begin, std::distance(uri_part_begin, uri_part_end));
    }

  } // namespace

  boost::optional<boost::string_ref> uri::scheme() const {
    return pimpl_->uri_parts_.scheme?
      to_string_ref(pimpl_->uri_, *pimpl_->uri_parts_.scheme)
      : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::user_info() const {
    return pimpl_->uri_parts_.hier_part.user_info?
      to_string_ref(pimpl_->uri_, *pimpl_->uri_parts_.hier_part.user_info)
      : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::host() const {
    return pimpl_->uri_parts_.hier_part.host?
      to_string_ref(pimpl_->uri_, *pimpl_->uri_parts_.hier_part.host)
      : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::port() const {
    return pimpl_->uri_parts_.hier_part.port?
      to_string_ref(pimpl_->uri_, *pimpl_->uri_parts_.hier_part.port)
      : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::path() const {
    return pimpl_->uri_parts_.hier_part.path?
      to_string_ref(pimpl_->uri_, *pimpl_->uri_parts_.hier_part.path)
      : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::query() const {
    return pimpl_->uri_parts_.query ?
      to_string_ref(pimpl_->uri_, *pimpl_->uri_parts_.query)
      : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::fragment() const {
    return pimpl_->uri_parts_.fragment?
      to_string_ref(pimpl_->uri_, *pimpl_->uri_parts_.fragment)
      : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::authority() const {
    auto host = this->host();
    if (!host) {
      return boost::optional<boost::string_ref>();
    }

    auto first = std::begin(*host), last = std::end(*host);
    auto user_info = this->user_info();
    if (user_info) {
      first = std::begin(*user_info);
    }

    auto port = this->port();
    if (port) {
      last = std::end(*port);
    }

    return to_string_ref(first, last);
  }

  uri::string_type uri::native() const {
    return pimpl_->uri_;
  }

  std::string uri::string() const {
    return pimpl_->uri_;
  }

  std::wstring uri::wstring() const {
    return std::wstring(std::begin(pimpl_->uri_), std::end(pimpl_->uri_));
  }

  std::u16string uri::u16string() const {
    return std::u16string(std::begin(pimpl_->uri_), std::end(pimpl_->uri_));
  }

  std::u32string uri::u32string() const {
    return std::u32string(std::begin(pimpl_->uri_), std::end(pimpl_->uri_));
  }

  bool uri::empty() const NETWORK_URI_NOEXCEPT {
    return pimpl_->uri_.empty();
  }

  bool uri::is_absolute() const NETWORK_URI_NOEXCEPT {
    return static_cast<bool>(scheme());
  }

  bool uri::is_opaque() const NETWORK_URI_NOEXCEPT {
    return (is_absolute() && !authority());
  }

  uri uri::normalize(uri_comparison_level level) const {
    string_type normalized(pimpl_->uri_);
    detail::uri_parts<string_type::iterator> parts;
    advance_parts(normalized, parts, pimpl_->uri_parts_);

    if ((uri_comparison_level::case_normalization == level) ||
	(uri_comparison_level::percent_encoding_normalization == level) ||
	(uri_comparison_level::path_segment_normalization == level)) {
      // All alphabetic characters in the scheme and host are
      // lower-case...
      if (parts.scheme) {
	boost::to_lower(*parts.scheme);
      }

      if (parts.hier_part.host) {
	boost::to_lower(*parts.hier_part.host);
      }

      // ...except when used in percent encoding
      boost::for_each(normalized, detail::normalize_percent_encoded());
    }

    if ((uri_comparison_level::percent_encoding_normalization == level) ||
	(uri_comparison_level::path_segment_normalization == level)) {
      // parts are invalidated here
      // there's got to be a better way of doing this that doesn't
      // mean parsing again (twice!)
      normalized.erase(detail::decode_encoded_chars(std::begin(normalized), std::end(normalized)),
		       std::end(normalized));
    }

    if (uri_comparison_level::path_segment_normalization == level) {
      // need to parse the parts again as the underlying string has changed
      bool is_valid = detail::parse(std::begin(normalized), std::end(normalized), parts);
      assert(is_valid);

      if (parts.hier_part.path) {
	uri::string_type path = detail::normalize_path_segments(
								to_string_ref(normalized, *parts.hier_part.path));

	// put the normalized path back into the uri
	boost::optional<string_type> query, fragment;
	if (parts.query) {
	  query.reset(string_type(std::begin(*parts.query), std::end(*parts.query)));
	}

	if (parts.fragment) {
	  fragment.reset(string_type(std::begin(*parts.fragment), std::end(*parts.fragment)));
	}

	string_type::iterator path_begin = std::begin(normalized);
	std::advance(path_begin,
		     std::distance<string_type::iterator>(path_begin,
							  std::begin(*parts.hier_part.path)));
	normalized.erase(path_begin, std::end(normalized));
	normalized.append(path);

	if (query) {
	  normalized.append("?");
	  normalized.append(*query);
	}

	if (fragment) {
	  normalized.append("#");
	  normalized.append(*fragment);
	}
      }
    }

    return uri(normalized);
  }

  uri uri::make_reference(const uri &other, uri_comparison_level level) const {
    if (is_opaque() || other.is_opaque()) {
      return other;
    }

    auto scheme = this->scheme(), other_scheme = other.scheme();
    if ((!scheme || !other_scheme) ||
	!boost::algorithm::equals(*scheme, *other_scheme)) {
      return other;
    }

    auto authority = this->authority(), other_authority = other.authority();
    if ((!authority || !other_authority) ||
	!boost::algorithm::equals(*authority, *other_authority)) {
      return other;
    }

    if (!this->path() || !other.path()) {
      return other;
    }

    auto path = detail::normalize_path(*this->path(), level),
      other_path = detail::normalize_path(*other.path(), level);

    boost::optional<string_type> query, fragment;
    if (other.query()) {
      query = uri::string_type(std::begin(*other.query()), std::end(*other.query()));
    }

    if (other.fragment()) {
      fragment = uri::string_type(std::begin(*other.fragment()), std::end(*other.fragment()));
    }

    network::uri result;
    result.initialize(boost::optional<string_type>(),
		      boost::optional<string_type>(),
		      boost::optional<string_type>(),
		      boost::optional<string_type>(),
		      other_path, query, fragment);
    return std::move(result);
  }

  namespace detail {
    template<typename T>
    inline boost::optional<uri::string_type>
    make_arg(T&& arg) {
      return boost::optional<uri::string_type>(std::forward<T>(arg));
    }

    inline boost::optional<uri::string_type>
    make_arg(boost::optional<boost::string_ref> ref) {
      if (ref) {
        return uri::string_type(std::begin(*ref), std::end(*ref));
      }
      return boost::optional<uri::string_type>();
    }
  }  // namespace detail

  uri uri::resolve(const uri &reference, uri_comparison_level level) const {
    // This implementation uses the psuedo-code given in
    // http://tools.ietf.org/html/rfc3986#section-5.2.2

    if (reference.is_absolute() && !reference.is_opaque()) {
      return reference;
    }

    if (reference.is_opaque()) {
      return reference;
    }

    boost::optional<uri::string_type>
      user_info, host, port, path, query;
    const uri& base = *this;

    if (reference.authority()) {
      // g -> http://g
      user_info = detail::make_arg(reference.user_info());
      host = detail::make_arg(reference.host());
      port = detail::make_arg(reference.port());
      path = detail::remove_dot_segments(*reference.path());
      query = detail::make_arg(reference.query());
    }
    else {
      if (!reference.path() || reference.path()->empty()) {
        path = detail::make_arg(base.path());
        if (reference.query()) {
          query = detail::make_arg(reference.query());
	}
	else {
          query = detail::make_arg(base.query());
	}
      }
      else {
        if (reference.path().get().front() == '/') {
          path = detail::remove_dot_segments(*reference.path());
	}
        else {
          path = detail::merge_paths(base, reference);
        }
        query = detail::make_arg(reference.query());
      }
      user_info = detail::make_arg(base.user_info());
      host = detail::make_arg(base.host());
      port = detail::make_arg(base.port());
    }

    network::uri result;
    result.initialize(detail::make_arg(base.scheme()),
		      std::move(user_info), std::move(host), std::move(port),
		      std::move(path), std::move(query),
		      detail::make_arg(reference.fragment()));
    return std::move(result);
  }

  int uri::compare(const uri &other, uri_comparison_level level) const {
    // if both URIs are empty, then we should define them as equal
    // even though they're still invalid.
    if (empty() && other.empty()) {
      return 0;
    }

    if (empty()) {
      return -1;
    }

    if (other.empty()) {
      return 1;
    }

    return normalize(level).native().compare(other.normalize(level).native());
  }

  bool uri::initialize(const string_type &uri) {
    pimpl_ = new (std::nothrow) impl;
    assert(pimpl_ && "Unable to allocate memory for network::uri");

    pimpl_->uri_ = boost::trim_copy(uri);
    if (!pimpl_->uri_.empty()) {
      auto first = std::begin(pimpl_->uri_), last = std::end(pimpl_->uri_);
      bool is_valid = detail::parse(first, last, pimpl_->uri_parts_);
      return is_valid;
    }
    return true;
  }

  void swap(uri &lhs, uri &rhs) NETWORK_URI_NOEXCEPT {
    lhs.swap(rhs);
  }

  bool operator == (const uri &lhs, const uri &rhs) {
    return lhs.compare(rhs, uri_comparison_level::path_segment_normalization) == 0;
  }

  bool operator == (const uri &lhs, const char *rhs) {
    if (std::strlen(rhs) != std::size_t(std::distance(std::begin(lhs), std::end(lhs)))) {
      return false;
    }
    return std::equal(std::begin(lhs), std::end(lhs), rhs);
  }

  bool operator < (const uri &lhs, const uri &rhs) {
    return lhs.compare(rhs, uri_comparison_level::path_segment_normalization) < 0;
  }
} // namespace network
