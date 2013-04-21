// Copyright (c) Glyn Matthews 2012, 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <gtest/gtest.h>
#include <network/uri.hpp>

using namespace network;

struct uri_resolve_test : public ::testing::Test {
  uri_resolve_test() : base_uri("http://a/b/c/d;p?q")
  {
  }

  uri::string_type resolved(const network::uri& base, uri::string_type reference)
  {
    uri reference_uri(reference);
    return base.resolve(reference_uri, 
      network::uri_comparison_level::string_comparison).string();
  }

  uri::string_type resolved(uri::string_type base, uri::string_type reference)
  {
    return resolved(uri(base), reference);
  }

  template<typename String>
  uri::string_type resolved(String&& reference)
  {
    return resolved(base_uri, std::forward<String>(reference));
  }

  network::uri base_uri;
};

// additional cases

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
//      "./g"           =  "http://a/b/c/g"

//      "g/"            =  "http://a/b/c/g/"
TEST_F(uri_resolve_test, base_has_path__path_is_merged)
{
  ASSERT_EQ("http://a/b/c/g/", resolved("g/"));
  ASSERT_EQ("http://a/b/c/g", resolved("g"));

}


//      "/g"            =  "http://a/g"
TEST_F(uri_resolve_test, path_starts_with_slash__path_is_ref_path)
{
  ASSERT_EQ("http://a/g", resolved("/g"));
}

//      "/g/x?y#s"            =  "http://a/g/x?y#s"
TEST_F(uri_resolve_test, path_starts_with_slash_with_query_fragment__path_is_ref_path)
{
  ASSERT_EQ("http://a/g/x?y#s", resolved("/g/x?y#s"));
}

//      "//g"           =  "http://g"
TEST_F(uri_resolve_test, has_authority__base_scheme_with_ref_authority)
{
  ASSERT_EQ("http://g", resolved("//g"));
}

//      "?y"            =  "http://a/b/c/d;p?y"
TEST_F(uri_resolve_test, path_is_empty_but_has_query__returns_base_with_ref_query)
{
    ASSERT_EQ("http://a/b/c/d;p?y", resolved("?y"));
}

TEST_F(uri_resolve_test, path_is_empty_but_has_query_base_no_query__returns_base_with_ref_query)
{
    ASSERT_EQ( "http://a/b/c/d?y", resolved("http://a/b/c/d", "?y"));
}

//      "g?y"           =  "http://a/b/c/g?y"
//      "#s"            =  "http://a/b/c/d;p?q#s"
//      "g#s"           =  "http://a/b/c/g#s"
//      "g?y#s"         =  "http://a/b/c/g?y#s"
//      ";x"            =  "http://a/b/c/;x"
//      "g;x"           =  "http://a/b/c/g;x"
//      "g;x?y#s"       =  "http://a/b/c/g;x?y#s"


//      ""              =  "http://a/b/c/d;p?q"
TEST_F(uri_resolve_test, path_is_empty__returns_base)
{
    ASSERT_EQ("http://a/b/c/d;p?q", resolved(""));
}

//      "."             =  "http://a/b/c/"
//      "./"            =  "http://a/b/c/"
//      ".."            =  "http://a/b/"
//      "../"           =  "http://a/b/"
//      "../g"          =  "http://a/b/g"
//      "../.."         =  "http://a/"
//      "../../"        =  "http://a/"
//      "../../g"       =  "http://a/g"




// abnormal examples
// http://tools.ietf.org/html/rfc3986#section-5.4.2

//      "../../../g"    =  "http://a/g"
//      "../../../../g" =  "http://a/g"
//      "/./g"          =  "http://a/g"
//      "/../g"         =  "http://a/g"
//      "g."            =  "http://a/b/c/g."
//      ".g"            =  "http://a/b/c/.g"
//      "g.."           =  "http://a/b/c/g.."
//      "..g"           =  "http://a/b/c/..g"
//      "./../g"        =  "http://a/b/g"
//      "./g/."         =  "http://a/b/c/g/"
//      "g/./h"         =  "http://a/b/c/g/h"
//      "g/../h"        =  "http://a/b/c/h"
//      "g;x=1/./y"     =  "http://a/b/c/g;x=1/y"
//      "g;x=1/../y"    =  "http://a/b/c/y"
//      "g?y/./x"       =  "http://a/b/c/g?y/./x"
//      "g?y/../x"      =  "http://a/b/c/g?y/../x"
//      "g#s/./x"       =  "http://a/b/c/g#s/./x"
//      "g#s/../x"      =  "http://a/b/c/g#s/../x"
