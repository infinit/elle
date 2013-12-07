// Copyright 2012, 2013 Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <cctype>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>
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
#include "network/uri/uri.hpp"
#include "detail/uri_parse.hpp"
#include "detail/uri_percent_encode.hpp"
#include "detail/uri_normalize.hpp"
#include "detail/uri_resolve.hpp"

namespace network {
  namespace {
    inline boost::iterator_range<uri::string_type::iterator> copy_range(
        uri::string_type::iterator first, uri::string_type::iterator last,
        uri::string_type::iterator &it) {
      auto part_first = it;
      std::advance(it, std::distance(first, last));
      return boost::make_iterator_range(part_first, it);
    }

    void advance_parts(
        uri::string_type::iterator first, uri::string_type::iterator last,
        detail::uri_parts<uri::string_type::iterator> &parts,
        const detail::uri_parts<uri::string_type::iterator> &existing_parts) {
      auto it = first;
      if (auto scheme = existing_parts.scheme) {
        parts.scheme.reset(
            copy_range(std::begin(*scheme), std::end(*scheme), it));

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
        parts.hier_part.user_info.reset(
            copy_range(std::begin(*user_info), std::end(*user_info), it));
        ++it;  // ignore @
      }

      if (auto host = existing_parts.hier_part.host) {
        parts.hier_part.host.reset(
            copy_range(std::begin(*host), std::end(*host), it));
      }

      if (auto port = existing_parts.hier_part.port) {
        ++it;  // ignore :
        parts.hier_part.port.reset(
            copy_range(std::begin(*port), std::end(*port), it));
      }

      if (auto path = existing_parts.hier_part.path) {
        parts.hier_part.path.reset(
            copy_range(std::begin(*path), std::end(*path), it));
      }

      if (auto query = existing_parts.query) {
        ++it;  // ignore ?
        parts.query.reset(copy_range(std::begin(*query), std::end(*query), it));
      }

      if (auto fragment = existing_parts.fragment) {
        ++it;  // ignore #
        parts.fragment.reset(
            copy_range(std::begin(*fragment), std::end(*fragment), it));
      }
    }

    void advance_parts(
        boost::iterator_range<uri::string_type::iterator> range,
        detail::uri_parts<uri::string_type::iterator> &parts,
        const detail::uri_parts<uri::string_type::iterator> &existing_parts) {
      advance_parts(std::begin(range), std::end(range), parts, existing_parts);
    }
  }  // namespace

  void uri::initialize(boost::optional<string_type> scheme,
                       boost::optional<string_type> user_info,
                       boost::optional<string_type> host,
                       boost::optional<string_type> port,
                       boost::optional<string_type> path,
                       boost::optional<string_type> query,
                       boost::optional<string_type> fragment) {
    if (scheme) {
      uri_.append(*scheme);
    }

    if (user_info || host || port) {
      if (scheme) {
        uri_.append("://");
      }

      if (user_info) {
        uri_.append(*user_info);
        uri_.append("@");
      }

      if (host) {
        uri_.append(*host);
      } else {
        throw uri_builder_error();
      }

      if (port) {
        uri_.append(":");
        uri_.append(*port);
      }
    } else {
      if (scheme) {
        if (path || query || fragment) {
          uri_.append(":");
        } else {
          throw uri_builder_error();
        }
      }
    }

    if (path) {
      uri_.append(*path);
    }

    if (query) {
      uri_.append("?");
      uri_.append(*query);
    }

    if (fragment) {
      uri_.append("#");
      uri_.append(*fragment);
    }

    auto it = std::begin(uri_);
    if (scheme) {
      uri_parts_.scheme.reset(
          copy_range(std::begin(*scheme), std::end(*scheme), it));
      // ignore ://
      while ((':' == *it) || ('/' == *it)) {
        ++it;
      }
    }

    if (user_info) {
      uri_parts_.hier_part.user_info.reset(
          copy_range(std::begin(*user_info), std::end(*user_info), it));
      ++it;  // ignore @
    }

    if (host) {
      uri_parts_.hier_part.host.reset(
          copy_range(std::begin(*host), std::end(*host), it));
    }

    if (port) {
      ++it;  // ignore :
      uri_parts_.hier_part.port.reset(
          copy_range(std::begin(*port), std::end(*port), it));
    }

    if (path) {
      uri_parts_.hier_part.path.reset(
          copy_range(std::begin(*path), std::end(*path), it));
    }

    if (query) {
      ++it;  // ignore ?
      uri_parts_.query.reset(
          copy_range(std::begin(*query), std::end(*query), it));
    }

    if (fragment) {
      ++it;  // ignore #
      uri_parts_.fragment.reset(
          copy_range(std::begin(*fragment), std::end(*fragment), it));
    }
  }

  uri::uri() {}

  uri::uri(const uri &other) : uri_(other.uri_) {
    advance_parts(uri_, uri_parts_, other.uri_parts_);
  }

  uri::uri(const uri_builder &builder) {
    initialize(builder.scheme_, builder.user_info_, builder.host_,
               builder.port_, builder.path_, builder.query_, builder.fragment_);
  }

  uri::uri(uri &&other) NETWORK_URI_NOEXCEPT : uri_(std::move(other.uri_)) {
    advance_parts(uri_, uri_parts_, other.uri_parts_);
    other.uri_.clear();
    other.uri_parts_ = detail::uri_parts<string_type::iterator>();
  }

  uri::~uri() NETWORK_URI_NOEXCEPT {}

  uri &uri::operator=(uri other) {
    other.swap(*this);
    return *this;
  }

  void uri::swap(uri &other) NETWORK_URI_NOEXCEPT {
    advance_parts(other.uri_, uri_parts_, other.uri_parts_);
    uri_.swap(other.uri_);
    advance_parts(other.uri_, other.uri_parts_, uri_parts_);
  }

  uri::const_iterator uri::begin() const { return uri_.begin(); }

  uri::const_iterator uri::end() const { return uri_.end(); }

  namespace {
    inline boost::string_ref to_string_ref(
        const uri::string_type &uri,
        boost::iterator_range<uri::const_iterator> uri_part) {
      if (!uri_part.empty()) {
        const char *c_str = uri.c_str();
        const char *uri_part_begin = &(*(std::begin(uri_part)));
        std::advance(c_str, std::distance(c_str, uri_part_begin));
        return boost::string_ref(
            c_str, std::distance(std::begin(uri_part), std::end(uri_part)));
      }
      return boost::string_ref();
    }

    inline boost::string_ref to_string_ref(
        boost::string_ref::const_iterator uri_part_begin,
        boost::string_ref::const_iterator uri_part_end) {
      return boost::string_ref(uri_part_begin,
                               std::distance(uri_part_begin, uri_part_end));
    }

  }  // namespace

  boost::optional<boost::string_ref> uri::scheme() const {
    return uri_parts_.scheme ? to_string_ref(uri_, *uri_parts_.scheme)
                             : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::user_info() const
      {
    return uri_parts_.hier_part.user_info
               ? to_string_ref(uri_, *uri_parts_.hier_part.user_info)
               : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::host() const {
    return uri_parts_.hier_part.host
               ? to_string_ref(uri_, *uri_parts_.hier_part.host)
               : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::port() const {
    return uri_parts_.hier_part.port
               ? to_string_ref(uri_, *uri_parts_.hier_part.port)
               : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::path() const {
    return uri_parts_.hier_part.path
               ? to_string_ref(uri_, *uri_parts_.hier_part.path)
               : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::query() const {
    return uri_parts_.query ? to_string_ref(uri_, *uri_parts_.query)
                            : boost::optional<boost::string_ref>();
  }

  boost::optional<boost::string_ref> uri::fragment() const {
    return uri_parts_.fragment ? to_string_ref(uri_, *uri_parts_.fragment)
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

  std::string uri::string() const { return uri_; }

  std::wstring uri::wstring() const {
    return std::wstring(std::begin(uri_), std::end(uri_));
  }

  std::u16string uri::u16string() const {
    return std::u16string(std::begin(uri_), std::end(uri_));
  }

  std::u32string uri::u32string() const {
    return std::u32string(std::begin(uri_), std::end(uri_));
  }

  bool uri::empty() const NETWORK_URI_NOEXCEPT { return uri_.empty(); }

  bool uri::is_absolute() const {
    return static_cast<bool>(scheme());
  }

  bool uri::is_opaque() const {
    return (is_absolute() && !authority());
  }

  uri uri::normalize(uri_comparison_level level) const {
    string_type normalized(uri_);
    detail::uri_parts<string_type::iterator> parts;
    advance_parts(normalized, parts, uri_parts_);

    if (uri_comparison_level::syntax_based == level) {
      // All alphabetic characters in the scheme and host are
      // lower-case...
      if (parts.scheme) {
        boost::to_lower(*parts.scheme);
      }

      if (parts.hier_part.host) {
        boost::to_lower(*parts.hier_part.host);
      }

      // ...except when used in percent encoding
      boost::for_each(normalized, detail::percent_encoded_to_upper());

      // parts are invalidated here
      // there's got to be a better way of doing this that doesn't
      // mean parsing again (twice!)
      normalized.erase(detail::decode_encoded_chars(std::begin(normalized),
                                                    std::end(normalized)),
                       std::end(normalized));

      // need to parse the parts again as the underlying string has changed
      bool is_valid =
          detail::parse(std::begin(normalized), std::end(normalized), parts);
      assert(is_valid);

      if (parts.hier_part.path) {
        uri::string_type path = detail::normalize_path_segments(
            to_string_ref(normalized, *parts.hier_part.path));

        // put the normalized path back into the uri
        boost::optional<string_type> query, fragment;
        if (parts.query) {
          query.reset(
              string_type(std::begin(*parts.query), std::end(*parts.query)));
        }

        if (parts.fragment) {
          fragment.reset(string_type(std::begin(*parts.fragment),
                                     std::end(*parts.fragment)));
        }

        string_type::iterator path_begin = std::begin(normalized);
        std::advance(path_begin,
                     std::distance<string_type::iterator>(
                         path_begin, std::begin(*parts.hier_part.path)));
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

  uri uri::make_relative(const uri &other) const {
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

    auto path = detail::normalize_path(*this->path(),
                                       uri_comparison_level::syntax_based),
         other_path = detail::normalize_path(
             *other.path(), uri_comparison_level::syntax_based);

    boost::optional<string_type> query, fragment;
    if (other.query()) {
      query = uri::string_type(std::begin(*other.query()),
                               std::end(*other.query()));
    }

    if (other.fragment()) {
      fragment = uri::string_type(std::begin(*other.fragment()),
                                  std::end(*other.fragment()));
    }

    network::uri result;
    result.initialize(
        boost::optional<string_type>(), boost::optional<string_type>(),
        boost::optional<string_type>(), boost::optional<string_type>(),
        other_path, query, fragment);
    return std::move(result);
  }

  namespace detail {
    template <typename T>
    inline boost::optional<uri::string_type> make_arg(T &&arg) {
      return boost::optional<uri::string_type>(std::forward<T>(arg));
    }

    inline boost::optional<uri::string_type> make_arg(
        boost::optional<boost::string_ref> ref) {
      if (ref) {
        return uri::string_type(std::begin(*ref), std::end(*ref));
      }
      return boost::optional<uri::string_type>();
    }
  }  // namespace detail

  uri uri::resolve(const uri &base) const {
    // This implementation uses the psuedo-code given in
    // http://tools.ietf.org/html/rfc3986#section-5.2.2

    if (is_absolute() && !is_opaque()) {
      // throw an exception ?
      return *this;
    }

    if (is_opaque()) {
      // throw an exception ?
      return *this;
    }

    boost::optional<uri::string_type> user_info, host, port, path, query;
    //const uri &base = *this;

    if (this->authority()) {
      // g -> http://g
      user_info = detail::make_arg(this->user_info());
      host = detail::make_arg(this->host());
      port = detail::make_arg(this->port());
      path = detail::remove_dot_segments(*this->path());
      query = detail::make_arg(this->query());
    } else {
      if (!this->path() || this->path()->empty()) {
        path = detail::make_arg(base.path());
        if (this->query()) {
          query = detail::make_arg(this->query());
        } else {
          query = detail::make_arg(base.query());
        }
      } else {
        if (this->path().get().front() == '/') {
          path = detail::remove_dot_segments(*this->path());
        } else {
          path = detail::merge_paths(base, *this);
        }
        query = detail::make_arg(this->query());
      }
      user_info = detail::make_arg(base.user_info());
      host = detail::make_arg(base.host());
      port = detail::make_arg(base.port());
    }

    network::uri result;
    result.initialize(detail::make_arg(base.scheme()), std::move(user_info),
                      std::move(host), std::move(port), std::move(path),
                      std::move(query), detail::make_arg(this->fragment()));
    return std::move(result);
  }

  int uri::compare(const uri &other,
                   uri_comparison_level level) const NETWORK_URI_NOEXCEPT {
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

    return normalize(level).uri_.compare(other.normalize(level).uri_);
  }

  bool uri::initialize(const string_type &uri) {
    uri_ = boost::trim_copy(uri);
    if (!uri_.empty()) {
      auto first = std::begin(uri_), last = std::end(uri_);
      bool is_valid = detail::parse(first, last, uri_parts_);
      return is_valid;
    }
    return true;
  }

  void swap(uri &lhs, uri &rhs) NETWORK_URI_NOEXCEPT { lhs.swap(rhs); }

  bool operator==(const uri &lhs, const uri &rhs) NETWORK_URI_NOEXCEPT {
    return lhs.compare(rhs, uri_comparison_level::syntax_based) == 0;
  }

  bool operator==(const uri &lhs, const char *rhs) NETWORK_URI_NOEXCEPT {
    if (std::strlen(rhs) !=
        std::size_t(std::distance(std::begin(lhs), std::end(lhs)))) {
      return false;
    }
    return std::equal(std::begin(lhs), std::end(lhs), rhs);
  }

  bool operator<(const uri &lhs, const uri &rhs) NETWORK_URI_NOEXCEPT {
    return lhs.compare(rhs, uri_comparison_level::syntax_based) < 0;
  }
}  // namespace network
