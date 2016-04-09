// Copyright 2012-2016 Glyn Matthews.
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
#include "network/uri/uri.hpp"
#include "detail/uri_parts.hpp"
#include "detail/uri_parse.hpp"
#include "detail/uri_percent_encode.hpp"
#include "detail/uri_normalize.hpp"
#include "detail/uri_resolve.hpp"
#include "detail/algorithm.hpp"

namespace network {
namespace {
inline detail::iterator_pair copy_range(detail::iterator_pair rng,
                                        detail::iterator_pair::iterator &it) {
  auto first = std::begin(rng), last = std::end(rng);
  auto part_first = it;
  std::advance(it, std::distance(first, last));
  return detail::iterator_pair(part_first, it);
}

void advance_parts(std::string &range, detail::uri_parts &parts,
                   const detail::uri_parts &existing_parts) {
  auto first = std::begin(range), last = std::end(range);

  auto it = first;
  if (auto scheme = existing_parts.scheme) {
    parts.scheme = copy_range(*scheme, it);

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
    parts.hier_part.user_info = copy_range(*user_info, it);
    ++it;  // ignore @
  }

  if (auto host = existing_parts.hier_part.host) {
    parts.hier_part.host = copy_range(*host, it);
  }

  if (auto port = existing_parts.hier_part.port) {
    ++it;  // ignore :
    parts.hier_part.port = copy_range(*port, it);
  }

  if (auto path = existing_parts.hier_part.path) {
    parts.hier_part.path = copy_range(*path, it);
  }

  if (auto query = existing_parts.query) {
    ++it;  // ignore ?
    parts.query = copy_range(*query, it);
  }

  if (auto fragment = existing_parts.fragment) {
    ++it;  // ignore #
    parts.fragment = copy_range(*fragment, it);
  }
}

template <class Rng>
inline void to_lower(Rng &rng) {
  detail::transform(rng, std::begin(rng),
                    [](char ch) { return std::tolower(ch); });
}
}  // namespace

void uri::initialize(optional<string_type> scheme,
                     optional<string_type> user_info,
                     optional<string_type> host, optional<string_type> port,
                     optional<string_type> path, optional<string_type> query,
                     optional<string_type> fragment) {
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
    // if the URI is hierarchical and the path is not already
    // prefixed with a '/', add one.
    if (host && (!path->empty() && path->front() != '/')) {
      path = "/" + *path;
    }
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

  uri_view_ = string_view(uri_);

  auto it = std::begin(uri_);
  if (scheme) {
    uri_parts_->scheme = copy_range(*scheme, it);
    // ignore : and ://
    if (':' == *it) {
      ++it;
    }
    if ('/' == *it) {
      ++it;
      if ('/' == *it) {
        ++it;
      }
    }
  }

  if (user_info) {
    uri_parts_->hier_part.user_info = copy_range(*user_info, it);
    ++it;  // ignore @
  }

  if (host) {
    uri_parts_->hier_part.host = copy_range(*host, it);
  }

  if (port) {
    ++it;  // ignore :
    uri_parts_->hier_part.port = copy_range(*port, it);
  }

  if (path) {
    uri_parts_->hier_part.path =  copy_range(*path, it);
  }

  if (query) {
    ++it;  // ignore ?
    uri_parts_->query = copy_range(*query, it);
  }

  if (fragment) {
    ++it;  // ignore #
    uri_parts_->fragment = copy_range(*fragment, it);
  }
}

uri::uri() : uri_view_(uri_), uri_parts_(new detail::uri_parts{}) {}

uri::uri(const uri &other) : uri_(other.uri_), uri_view_(uri_), uri_parts_(new detail::uri_parts{}) {
  advance_parts(uri_, *uri_parts_, *other.uri_parts_);
}

uri::uri(const uri_builder &builder) : uri_parts_(new detail::uri_parts{}) {
  initialize(builder.scheme_, builder.user_info_, builder.host_, builder.port_,
             builder.path_, builder.query_, builder.fragment_);
}

uri::uri(uri &&other) noexcept : uri_(std::move(other.uri_)), uri_view_(uri_), uri_parts_(std::move(other.uri_parts_)) {
  advance_parts(uri_, *uri_parts_, *other.uri_parts_);
  other.uri_.clear();
  other.uri_view_ = string_view(other.uri_);
  other.uri_parts_ = new detail::uri_parts{};
}

uri::~uri() {
  delete uri_parts_;
}

uri &uri::operator=(uri other) {
  other.swap(*this);
  return *this;
}

void uri::swap(uri &other) noexcept {
  advance_parts(other.uri_, *uri_parts_, *other.uri_parts_);
  uri_.swap(other.uri_);
  uri_view_.swap(other.uri_view_);
  advance_parts(other.uri_, *other.uri_parts_, *uri_parts_);
}

uri::const_iterator uri::begin() const { return uri_view_.begin(); }

uri::const_iterator uri::end() const { return uri_view_.end(); }

namespace {
inline uri::string_view to_string_view(const uri::string_type &uri,
                                       detail::iterator_pair uri_part) {
  if (!uri_part.empty()) {
    const char *c_str = uri.c_str();
    const char *uri_part_begin = &(*(std::begin(uri_part)));
    std::advance(c_str, std::distance(c_str, uri_part_begin));
    return uri::string_view(c_str, detail::distance(uri_part));
  }
  return uri::string_view();
}

inline uri::string_view to_string_view(
    uri::string_view::const_iterator uri_part_begin,
    uri::string_view::const_iterator uri_part_end) {
  return uri::string_view(uri_part_begin,
                          std::distance(uri_part_begin, uri_part_end));
}
}  // namespace

bool uri::has_scheme() const {
  return static_cast<bool>(uri_parts_->scheme);
}

uri::string_view uri::scheme() const {
  assert(has_scheme());
  return to_string_view(uri_, *uri_parts_->scheme);
}

bool uri::has_user_info() const {
  return static_cast<bool>(uri_parts_->hier_part.user_info);
}

uri::string_view uri::user_info() const {
  assert(has_user_info());
  return to_string_view(uri_, *uri_parts_->hier_part.user_info);
}

bool uri::has_host() const {
  return static_cast<bool>(uri_parts_->hier_part.host);
}

uri::string_view uri::host() const {
  assert(has_host());
  return to_string_view(uri_, *uri_parts_->hier_part.host);
}

bool uri::has_port() const {
  return static_cast<bool>(uri_parts_->hier_part.port);
}

uri::string_view uri::port() const {
  assert(has_port());
  return to_string_view(uri_, *uri_parts_->hier_part.port);
}

bool uri::has_path() const {
  return static_cast<bool>(uri_parts_->hier_part.path);
}

uri::string_view uri::path() const {
  assert(has_path());
  return to_string_view(uri_, *uri_parts_->hier_part.path);
}

bool uri::has_query() const {
  return static_cast<bool>(uri_parts_->query);
}

uri::string_view uri::query() const {
  assert(has_query());
  return to_string_view(uri_, *uri_parts_->query);
}

bool uri::has_fragment() const {
  return static_cast<bool>(uri_parts_->fragment);
}

uri::string_view uri::fragment() const {
  assert(has_fragment());
  return to_string_view(uri_, *uri_parts_->fragment);
}

bool uri::has_authority() const {
  return has_host();
}

uri::string_view uri::authority() const {
  assert(has_host());
  auto host = this->host();

  auto user_info = string_view{};
  if (has_user_info()) {
    user_info = this->user_info();
  }

  auto port = string_view{};
  if (has_port()) {
    port = this->port();
  }

  auto first = std::begin(host), last = std::end(host);
  if (has_user_info() && !user_info.empty()) {
    first = std::begin(user_info);
  }
  else if (host.empty() && has_port() && !port.empty()) {
    first = std::begin(port);
    --first; // include ':' before port
  }

  if (host.empty()) {
    if (has_port() && !port.empty()) {
      last = std::end(port);
    }
    else if (has_user_info() && !user_info.empty()) {
      last = std::end(user_info);
      ++last; // include '@'
    }
  }
  else if (has_port()) {
    if (port.empty()) {
      ++last; // include ':' after host
    }
    else {
      last = std::end(port);
    }
  }

  return to_string_view(first, last);
}

std::string uri::string() const { return uri_; }

std::wstring uri::wstring() const {
  return std::wstring(std::begin(*this), std::end(*this));
}

std::u16string uri::u16string() const {
  return std::u16string(std::begin(*this), std::end(*this));
}

std::u32string uri::u32string() const {
  return std::u32string(std::begin(*this), std::end(*this));
}

bool uri::empty() const noexcept { return uri_.empty(); }

bool uri::is_absolute() const { return has_scheme(); }

bool uri::is_opaque() const { return (is_absolute() && !has_authority()); }

uri uri::normalize(uri_comparison_level level) const {
  string_type normalized(uri_);
  detail::uri_parts parts;
  advance_parts(normalized, parts, *uri_parts_);

  if (uri_comparison_level::syntax_based == level) {
    // All alphabetic characters in the scheme and host are
    // lower-case...
    if (parts.scheme) {
      to_lower(*parts.scheme);
    }

    if (parts.hier_part.host) {
      to_lower(*parts.hier_part.host);
    }

    // ...except when used in percent encoding
    detail::for_each(normalized, detail::percent_encoded_to_upper());

    // parts are invalidated here
    // there's got to be a better way of doing this that doesn't
    // mean parsing again (twice!)
    normalized.erase(detail::decode_encoded_unreserved_chars(std::begin(normalized),
                                                             std::end(normalized)),
                     std::end(normalized));

    // need to parse the parts again as the underlying string has changed
    bool is_valid = detail::parse(normalized, parts);
    assert(is_valid);

    if (parts.hier_part.path) {
      uri::string_type path = detail::normalize_path_segments(
          to_string_view(normalized, *parts.hier_part.path));

      // put the normalized path back into the uri
      optional<string_type> query, fragment;
      if (parts.query) {
        query = string_type(std::begin(*parts.query), std::end(*parts.query));
      }

      if (parts.fragment) {
        fragment = string_type(std::begin(*parts.fragment),
                               std::end(*parts.fragment));
      }

      auto path_begin = std::begin(normalized);
      std::advance(path_begin,
                   std::distance<detail::iterator_pair::iterator>(
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

  if ((!has_scheme() || !other.has_scheme()) ||
      !detail::equal(scheme(), other.scheme())) {
    return other;
  }

  if ((!has_authority() || !other.has_authority()) ||
      !detail::equal(authority(), other.authority())) {
    return other;
  }

  if (!has_path() || !other.has_path()) {
    return other;
  }

  auto path =
    detail::normalize_path(this->path(), uri_comparison_level::syntax_based);
  auto other_path = detail::normalize_path(other.path(),
                                           uri_comparison_level::syntax_based);

  optional<string_type> query, fragment;
  if (other.has_query()) {
    query = other.query().to_string();
  }

  if (other.has_fragment()) {
    fragment = other.fragment().to_string();
  }

  network::uri result;
  result.initialize(optional<string_type>(), optional<string_type>(),
                    optional<string_type>(), optional<string_type>(),
                    other_path, query, fragment);
  return std::move(result);
}

namespace detail {
inline optional<uri::string_type> make_arg(optional<uri::string_view> view) {
  if (view) {
    return view->to_string();
  }
  return nullopt;
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

  optional<uri::string_type> user_info, host, port, path, query, fragment;
  // const uri &base = *this;

  if (has_authority()) {
    // g -> http://g
    if (has_user_info()) {
      user_info = detail::make_arg(this->user_info());
    }

    if (has_host()) {
      host = detail::make_arg(this->host());
    }

    if (has_port()) {
      port = detail::make_arg(this->port());
    }

    if (has_path()) {
      path = detail::remove_dot_segments(this->path());
    }

    if (has_query()) {
      query = detail::make_arg(this->query());
    }
  } else {
    if (!has_path() || this->path().empty()) {
      if (base.has_path()) {
        path = detail::make_arg(base.path());
      }

      if (has_query()) {
        query = detail::make_arg(this->query());
      } else if (base.has_query()) {
        query = detail::make_arg(base.query());
      }
    } else {
      if (this->path().front() == '/') {
        path = detail::remove_dot_segments(this->path());
      } else {
        path = detail::merge_paths(base, *this);
      }

      if (has_query()) {
        query = detail::make_arg(this->query());
      }
    }

    if (base.has_user_info()) {
      user_info = detail::make_arg(base.user_info());
    }

    if (base.has_host()) {
      host = detail::make_arg(base.host());
    }

    if (base.has_port()) {
      port = detail::make_arg(base.port());
    }
  }

  if (has_fragment()) {
    fragment = detail::make_arg(this->fragment());
  }

  network::uri result;
  result.initialize(detail::make_arg(base.scheme()), std::move(user_info),
                    std::move(host), std::move(port), std::move(path),
                    std::move(query), std::move(fragment));
  return result;
}

int uri::compare(const uri &other, uri_comparison_level level) const noexcept {
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
  uri_ = detail::trim_copy(uri);
  uri_parts_ = new detail::uri_parts{};
  if (!uri_.empty()) {
    bool is_valid = detail::parse(uri_, *uri_parts_);
	uri_view_ = string_view(uri_);
    return is_valid;
  }
  return true;
}

void swap(uri &lhs, uri &rhs) noexcept { lhs.swap(rhs); }

bool operator==(const uri &lhs, const uri &rhs) noexcept {
  return lhs.compare(rhs, uri_comparison_level::syntax_based) == 0;
}

bool operator==(const uri &lhs, const char *rhs) noexcept {
  if (std::strlen(rhs) !=
      std::size_t(std::distance(std::begin(lhs), std::end(lhs)))) {
    return false;
  }
  return std::equal(std::begin(lhs), std::end(lhs), rhs);
}

bool operator<(const uri &lhs, const uri &rhs) noexcept {
  return lhs.compare(rhs, uri_comparison_level::syntax_based) < 0;
}
}  // namespace network
