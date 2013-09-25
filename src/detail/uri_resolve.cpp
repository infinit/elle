// Copyright 2013 Glyn Matthews.
// Copyright 2013 Hannes Kamecke.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "uri_resolve.hpp"

namespace network {
  namespace detail {
    // remove_dot_segments
    template <typename Range>
    void remove_last_segment(Range& path) {
      while (!path.empty()) {
        if (path.back() == '/') {
          path.pop_back();
          break;
        }
        path.pop_back();
      }
    }

    // implementation of http://tools.ietf.org/html/rfc3986#section-5.2.4
    uri::string_type remove_dot_segments(uri::string_type input) {
      using namespace boost::algorithm;
      using std::begin;

      uri::string_type output;
      while (!input.empty()) {
        if (starts_with(input, "../")) {
          erase_head(input, 3);
        } else if (starts_with(input, "./")) {
          erase_head(input, 2);
        } else if (starts_with(input, "/./")) {
          replace_head(input, 3, "/");
        } else if (input == "/.") {
          replace_head(input, 2, "/");
        } else if (starts_with(input, "/../")) {
          erase_head(input, 3);
          remove_last_segment(output);
        } else if (starts_with(input, "/..")) {
          replace_head(input, 3, "/");
          remove_last_segment(output);
        } else if (all(input, is_any_of("."))) {
          input.clear();
        } else {
          int n = input.front() == '/' ? 1 : 0;
          auto slash = find_nth(input, "/", n);
          output.append(begin(input), begin(slash));
          input.erase(begin(input), begin(slash));
        }
      }
      return output;
    }

    uri::string_type remove_dot_segments(uri::string_view path) {
      return remove_dot_segments(
          uri::string_type(std::begin(path), std::end(path)));
    }

    // implementation of http://tools.ietf.org/html/rfc3986#section-5.2.3
    uri::string_type merge_paths(const uri& base, const uri& reference) {
      uri::string_type path;
      if (!base.path() || base.path()->empty()) {
        path = "/";
      } else {
        const auto& base_path = base.path().get();
        auto last_slash = boost::find_last(base_path, "/");
        path.append(std::begin(base_path), last_slash.end());
      }
      if (reference.path()) {
        path.append(uri::string_type(std::begin(*reference.path()),
                                     std::end(*reference.path())));
      }
      return remove_dot_segments(std::move(path));
    }
  }  // namespace detail
}  // namespace network
