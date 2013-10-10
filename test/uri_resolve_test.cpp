// Copyright 2012, 2013 Glyn Matthews.
// Copyright 2013 Hannes Kamecke.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <gtest/gtest.h>
#include <network/uri.hpp>
#include "string_utility.hpp"

using namespace network;

struct uri_resolve_test : public ::testing::Test {
  uri_resolve_test() : base_uri("http://a/b/c/d;p?q")
  {
  }

  uri::string_type resolved(const network::uri& base, uri::string_type reference) {
    uri reference_uri(reference);
    return reference_uri.resolve(base).string();
  }

  uri::string_type resolved(uri::string_type base, uri::string_type reference) {
    return resolved(uri(base), reference);
  }

  template<typename String>
  uri::string_type resolved(String&& reference)
  {
    return resolved(base_uri, std::forward<String>(reference));
  }

  network::uri base_uri;
};


TEST_F(uri_resolve_test, is_absolute_uri__returns_other)
{
  ASSERT_EQ("https://www.example.com/", resolved("https://www.example.com/"));
}

TEST_F(uri_resolve_test, base_has_empty_path__path_is_ref_path)
{
  ASSERT_EQ("http://a/g", resolved("http://a/", "g"));
  ASSERT_EQ("http://a/g/x/y?q#s", resolved("http://a/", "g/x/y?q#s"));
}

// normal examples
// http://tools.ietf.org/html/rfc3986#section-5.4.1

TEST_F(uri_resolve_test, remove_dot_segments1) {
  ASSERT_EQ("http://a/b/c/g", resolved("./g"));
}

TEST_F(uri_resolve_test, base_has_path__path_is_merged) {
  ASSERT_EQ("http://a/b/c/g/", resolved("g/"));
  ASSERT_EQ("http://a/b/c/g", resolved("g"));
}

TEST_F(uri_resolve_test, path_starts_with_slash__path_is_ref_path) {
  ASSERT_EQ("http://a/g", resolved("/g"));
}

TEST_F(uri_resolve_test, path_starts_with_slash_with_query_fragment__path_is_ref_path) {
  ASSERT_EQ("http://a/g/x?y#s", resolved("/g/x?y#s"));
}

TEST_F(uri_resolve_test, has_authority__base_scheme_with_ref_authority) {
  ASSERT_EQ("http://g", resolved("//g"));
}

TEST_F(uri_resolve_test, path_is_empty_but_has_query__returns_base_with_ref_query) {
    ASSERT_EQ("http://a/b/c/d;p?y", resolved("?y"));
}

TEST_F(uri_resolve_test, path_is_empty_but_has_query_base_no_query__returns_base_with_ref_query) {
    ASSERT_EQ( "http://a/b/c/d?y", resolved("http://a/b/c/d", "?y"));
}

TEST_F(uri_resolve_test, merge_path_with_query) {
  ASSERT_EQ("http://a/b/c/g?y", resolved("g?y"));
}

TEST_F(uri_resolve_test, append_fragment) {
  ASSERT_EQ("http://a/b/c/d;p?q#s", resolved("#s"));
}

TEST_F(uri_resolve_test, merge_paths_with_fragment) {
  ASSERT_EQ("http://a/b/c/g#s", resolved("g#s"));
}

TEST_F(uri_resolve_test, merge_paths_with_query_and_fragment) {
  ASSERT_EQ("http://a/b/c/g?y#s", resolved("g?y#s"));
}

TEST_F(uri_resolve_test, merge_paths_with_semicolon) {
  ASSERT_EQ("http://a/b/c/;x", resolved(";x"));
  ASSERT_EQ("http://a/b/c/g;x", resolved("g;x"));
  ASSERT_EQ("http://a/b/c/g;x?y#s", resolved("g;x?y#s"));
}

TEST_F(uri_resolve_test, path_is_empty__returns_base) {
    ASSERT_EQ("http://a/b/c/d;p?q", resolved(""));
}

TEST_F(uri_resolve_test, remove_dot_segments2) {
  ASSERT_EQ("http://a/b/c/", resolved("."));
}

TEST_F(uri_resolve_test, remove_dot_segments3) {
  ASSERT_EQ("http://a/b/c/", resolved("./"));
}

TEST_F(uri_resolve_test, remove_dot_segments4) {
  ASSERT_EQ("http://a/b/", resolved(".."));
}

TEST_F(uri_resolve_test, remove_dot_segments5) {
  ASSERT_EQ("http://a/b/", resolved("../"));
}

TEST_F(uri_resolve_test, remove_dot_segments6) {
  ASSERT_EQ("http://a/b/g", resolved("../g"));
}

TEST_F(uri_resolve_test, remove_dot_segments7) {
  ASSERT_EQ("http://a/", resolved("../.."));
}

TEST_F(uri_resolve_test, remove_dot_segments8) {
  ASSERT_EQ("http://a/", resolved("../../"));
}

TEST_F(uri_resolve_test, remove_dot_segments9) {
  ASSERT_EQ("http://a/g", resolved("../../g"));
}


// abnormal examples
// http://tools.ietf.org/html/rfc3986#section-5.4.2

TEST_F(uri_resolve_test, abnormal_example_1) {
  ASSERT_EQ("http://a/g", resolved("../../../g"));
}

TEST_F(uri_resolve_test, abnormal_example_2) {
  ASSERT_EQ("http://a/g", resolved("../../../../g"));
}

TEST_F(uri_resolve_test, abnormal_example_3) {
  ASSERT_EQ("http://a/g", resolved("/./g"));
}

TEST_F(uri_resolve_test, abnormal_example_4) {
  ASSERT_EQ("http://a/g", resolved("/../g"));
}

TEST_F(uri_resolve_test, abnormal_example_5) {
  ASSERT_EQ("http://a/b/c/g.", resolved("g."));
}

TEST_F(uri_resolve_test, abnormal_example_6) {
  ASSERT_EQ("http://a/b/c/.g", resolved(".g"));
}

TEST_F(uri_resolve_test, abnormal_example_7) {
  ASSERT_EQ("http://a/b/c/g..", resolved("g.."));
}

TEST_F(uri_resolve_test, abnormal_example_8) {
  ASSERT_EQ("http://a/b/g", resolved("..g"));
}

TEST_F(uri_resolve_test, abnormal_example_9) {
  ASSERT_EQ("http://a/b/g", resolved("./../g"));
}

TEST_F(uri_resolve_test, abnormal_example_10) {
  ASSERT_EQ("http://a/b/c/g/", resolved("./g/."));
}

TEST_F(uri_resolve_test, abnormal_example_11) {
  ASSERT_EQ("http://a/b/c/g/h", resolved("g/./h"));
}

TEST_F(uri_resolve_test, abnormal_example_12) {
  ASSERT_EQ("http://a/b/c/h", resolved("g/../h"));
}

TEST_F(uri_resolve_test, abnormal_example_13) {
  ASSERT_EQ("http://a/b/c/g;x=1/y", resolved("g;x=1/./y"));
}

TEST_F(uri_resolve_test, abnormal_example_14) {
  ASSERT_EQ("http://a/b/c/y", resolved("g;x=1/../y"));
}

TEST_F(uri_resolve_test, abnormal_example_15) {
  ASSERT_EQ("http://a/b/c/g?y/./x", resolved("g?y/./x"));
}

TEST_F(uri_resolve_test, abnormal_example_16) {
  ASSERT_EQ("http://a/b/c/g?y/../x", resolved("g?y/../x"));
}

TEST_F(uri_resolve_test, abnormal_example_17) {
  ASSERT_EQ("http://a/b/c/g#s/./x", resolved("g#s/./x"));
}

TEST_F(uri_resolve_test, abnormal_example_18) {
  ASSERT_EQ("http://a/b/c/g#s/../x", resolved("g#s/../x"));
}

TEST_F(uri_resolve_test, issue_resolve_from_copy) {
  // https://github.com/cpp-netlib/uri/issues/15
  network::uri base("http://a.com/");
  network::uri uri("http:/example.com/path/");
  network::uri copy = uri;
  ASSERT_TRUE(copy.is_opaque());
  auto result = copy.resolve(base);
  ASSERT_EQ("http:/example.com/path/", result);
}

TEST_F(uri_resolve_test, issue_resolve_from_move) {
  // https://github.com/cpp-netlib/uri/issues/15
  network::uri base("http://a.com/");
  network::uri uri("http:/example.com/path/");
  network::uri copy = std::move(uri);
  ASSERT_TRUE(copy.is_opaque());
  auto result = copy.resolve(base);
  ASSERT_EQ("http:/example.com/path/", result);
}

TEST_F(uri_resolve_test, issue_15_resolve_from_copy_with_query) {
  // https://github.com/cpp-netlib/uri/issues/15
  network::uri base("http://a.com/");
  network::uri uri("http:/example.com/path/?query#fragment");
  network::uri copy = uri;
  ASSERT_TRUE(copy.is_opaque());
  auto result = copy.resolve(base);
  ASSERT_EQ("query", *uri.query());
  ASSERT_EQ("query", *copy.query());
  ASSERT_EQ("query", *result.query());
}

TEST_F(uri_resolve_test, issue_15_resolve_from_copy_with_fragment) {
  // https://github.com/cpp-netlib/uri/issues/15
  network::uri base("http://a.com/");
  network::uri uri("http:/example.com/path/?query#fragment");
  network::uri copy = uri;
  ASSERT_TRUE(copy.is_opaque());
  auto result = copy.resolve(base);
  ASSERT_EQ("fragment", *result.fragment());
}
