// Copyright 2010 Jeroen Habraken.
// Copyright 2009-2012 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <network/uri/uri.hpp>
#include <network/uri/uri_io.hpp>
#include <algorithm>
#include <memory>
#include <map>
#include <set>
#include <cstring>


namespace network {
  namespace test {
    // A couple of helper functions to make the tests easier to read.
    template <class Rng1, class Rng2>
    bool equal(const Rng1 &rng1, const Rng2 &rng2) {
      return std::equal(std::begin(rng1), std::end(rng1), std::begin(rng2));
    }

    std::pair<const char *, const char *> as_literal(const char *rng) {
      return std::make_pair(rng, rng + std::strlen(rng));
    }
  } // namespace test
} // namespace network


TEST(uri_test, basic_uri_scheme_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_EQ(instance.scheme().string(), "http");
}

TEST(uri_test, basic_uri_user_info_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_EQ(instance.user_info().string(), "");
}

TEST(uri_test, basic_uri_host_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_EQ(instance.host().string(), "www.example.com");
}

TEST(uri_test, basic_uri_port_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_EQ(instance.port().string(), "");
}

TEST(uri_test, basic_uri_path_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, basic_uri_query_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_EQ(instance.query().string(), "");
}

TEST(uri_test, basic_uri_fragment_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_EQ(instance.fragment().string(), "");
}

TEST(uri_test, basic_uri_value_semantics_test) {
  network::uri original;
  network::uri assigned;
  assigned = original;
  ASSERT_TRUE(original == assigned);
  assigned = network::uri("http://www.example.com/");
  ASSERT_TRUE(original != assigned);
  network::uri copy(assigned);
  ASSERT_TRUE(copy == assigned);
}

TEST(uri_test, basic_uri_range_scheme_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.scheme());
  ASSERT_TRUE(instance.begin() == boost::begin(instance.scheme()));
  ASSERT_TRUE(network::test::equal(instance.scheme(), network::test::as_literal("http")));
}

TEST(uri_test, basic_uri_range_user_info_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(!instance.user_info());
  ASSERT_TRUE(boost::begin(instance.host()) == boost::begin(instance.user_info()));
  ASSERT_TRUE(boost::begin(instance.host()) == boost::end(instance.user_info()));
}

TEST(uri_test, basic_uri_range_host_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.host());
  ASSERT_TRUE(network::test::equal(instance.host(), network::test::as_literal("www.example.com")));
}

TEST(uri_test, basic_uri_range_port_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(!instance.port());
  ASSERT_TRUE(boost::end(instance.host()) == boost::begin(instance.port()));
  ASSERT_TRUE(boost::end(instance.host()) == boost::end(instance.port()));
}

TEST(uri_test, basic_uri_range_path_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.path());
  ASSERT_TRUE(network::test::equal(instance.path(), network::test::as_literal("/")));
  ASSERT_TRUE(instance.end() == boost::end(instance.path()));
}

TEST(uri_test, basic_uri_range_query_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(!instance.query());
  ASSERT_TRUE(instance.end() == boost::begin(instance.query()));
  ASSERT_TRUE(instance.end() == boost::end(instance.query()));
}

TEST(uri_test, basic_uri_range_fragment_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(!instance.fragment());
  ASSERT_TRUE(instance.end() == boost::begin(instance.fragment()));
  ASSERT_TRUE(instance.end() == boost::end(instance.fragment()));
}

TEST(uri_test, full_uri_scheme_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(instance.scheme().string(), "http");
}

TEST(uri_test, full_uri_user_info_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(instance.user_info().string(), "user:password");
}

TEST(uri_test, full_uri_host_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(instance.host().string(), "www.example.com");
}

TEST(uri_test, full_uri_port_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(instance.port().string(), "80");
}

TEST(uri_test, full_uri_path_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(instance.path().string(), "/path");
}

TEST(uri_test, full_uri_query_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(instance.query().string(), "query");
}

TEST(uri_test, full_uri_fragment_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(instance.fragment().string(), "fragment");
}

TEST(uri_test, full_uri_range_scheme_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.scheme());
  ASSERT_TRUE(instance.begin() == boost::begin(instance.scheme()));
  ASSERT_TRUE(network::test::equal(instance.scheme(), network::test::as_literal("http")));
}

TEST(uri_test, full_uri_range_user_info_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.user_info());
  ASSERT_TRUE(network::test::equal(instance.user_info(), network::test::as_literal("user:password")));
}

TEST(uri_test, full_uri_range_host_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.host());
  ASSERT_TRUE(network::test::equal(instance.host(), network::test::as_literal("www.example.com")));
}

TEST(uri_test, full_uri_range_port_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.port());
  ASSERT_TRUE(network::test::equal(instance.port(), network::test::as_literal("80")));
}

TEST(uri_test, full_uri_range_path_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.path());
  ASSERT_TRUE(network::test::equal(instance.path(), network::test::as_literal("/path")));
}

TEST(uri_test, full_uri_range_query_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.query());
  ASSERT_TRUE(network::test::equal(instance.query(), network::test::as_literal("query")));
}

TEST(uri_test, full_uri_range_fragment_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.fragment());
  ASSERT_TRUE(network::test::equal(instance.fragment(), network::test::as_literal("fragment")));
  ASSERT_TRUE(instance.end() == boost::end(instance.fragment()));
}

TEST(uri_test, mailto_test) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_EQ(instance.scheme().string(), "mailto");
  ASSERT_EQ(instance.path().string(), "john.doe@example.com");
}

TEST(uri_test, file_test) {
  network::uri instance("file:///bin/bash");
  ASSERT_EQ(instance.scheme().string(), "file");
  ASSERT_EQ(instance.path().string(), "/bin/bash");
}

TEST(uri_test, xmpp_test) {
  network::uri instance("xmpp:example-node@example.com?message;subject=Hello%20World");
  ASSERT_EQ(instance.scheme().string(), "xmpp");
  ASSERT_EQ(instance.path().string(), "example-node@example.com");
  ASSERT_EQ(instance.query().string(), "message;subject=Hello%20World");
}

TEST(uri_test, ipv4_address_test) {
  network::uri instance("http://129.79.245.252/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "129.79.245.252");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv4_loopback_test) {
  network::uri instance("http://127.0.0.1/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "127.0.0.1");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_1) {
  network::uri instance("http://[1080:0:0:0:8:800:200C:417A]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[1080:0:0:0:8:800:200C:417A]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_2) {
  network::uri instance("http://[2001:db8:85a3:8d3:1319:8a2e:370:7348]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[2001:db8:85a3:8d3:1319:8a2e:370:7348]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_3) {
  network::uri instance("http://[2001:db8:85a3:0:0:8a2e:370:7334]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[2001:db8:85a3:0:0:8a2e:370:7334]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_4) {
  network::uri instance("http://[2001:db8:85a3::8a2e:370:7334]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[2001:db8:85a3::8a2e:370:7334]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_5) {
  network::uri instance("http://[2001:0db8:0000:0000:0000:0000:1428:57ab]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[2001:0db8:0000:0000:0000:0000:1428:57ab]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_6) {
  network::uri instance("http://[2001:0db8:0000:0000:0000::1428:57ab]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[2001:0db8:0000:0000:0000::1428:57ab]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_7) {
  network::uri instance("http://[2001:0db8:0:0:0:0:1428:57ab]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[2001:0db8:0:0:0:0:1428:57ab]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_8) {
  network::uri instance("http://[2001:0db8:0:0::1428:57ab]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[2001:0db8:0:0::1428:57ab]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_9) {
  network::uri instance("http://[2001:0db8::1428:57ab]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[2001:0db8::1428:57ab]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_10) {
  network::uri instance("http://[2001:db8::1428:57ab]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[2001:db8::1428:57ab]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_11) {
  network::uri instance("http://[::ffff:0c22:384e]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[::ffff:0c22:384e]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_12) {
  network::uri instance("http://[fe80::]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[fe80::]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_address_test_13) {
  network::uri instance("http://[::ffff:c000:280]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[::ffff:c000:280]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_loopback_test) {
  network::uri instance("http://[::1]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[::1]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_loopback_test_1) {
  network::uri instance("http://[0000:0000:0000:0000:0000:0000:0000:0001]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[0000:0000:0000:0000:0000:0000:0000:0001]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_v4inv6_test_1) {
  network::uri instance("http://[::ffff:12.34.56.78]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[::ffff:12.34.56.78]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ipv6_v4inv6_test_2) {
  network::uri instance("http://[::ffff:192.0.2.128]/");
  ASSERT_EQ(instance.scheme().string(), "http");
  ASSERT_EQ(instance.host().string(), "[::ffff:192.0.2.128]");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, ftp_test) {
  network::uri instance("ftp://john.doe@ftp.example.com/");
  ASSERT_EQ(instance.scheme().string(), "ftp");
  ASSERT_EQ(instance.user_info().string(), "john.doe");
  ASSERT_EQ(instance.host().string(), "ftp.example.com");
  ASSERT_EQ(instance.path().string(), "/");
}

TEST(uri_test, news_test) {
  network::uri instance("news:comp.infosystems.www.servers.unix");
  ASSERT_EQ(instance.scheme().string(), "news");
  ASSERT_EQ(instance.path().string(), "comp.infosystems.www.servers.unix");
}

TEST(uri_test, tel_test) {
  network::uri instance("tel:+1-816-555-1212");
  ASSERT_EQ(instance.scheme().string(), "tel");
  ASSERT_EQ(instance.path().string(), "+1-816-555-1212");
}

//TEST(uri_test, encoded_uri_test) {
//  network::uri instance("http://www.example.com/Path%20With%20%28Some%29%20Encoded%20Characters%21");
//  ASSERT_EQ(instance.scheme().string(), "http");
//  ASSERT_EQ(instance.host().string(), "www.example.com");
//  ASSERT_EQ(instance.path().string(), "/Path%20With%20%28Some%29%20Encoded%20Characters%21");
//  ASSERT_EQ(network::decoded_path(instance), "/Path With (Some) Encoded Characters!");
//}

TEST(uri_test, copy_constructor_test) {
  network::uri instance("http://www.example.com/");
  network::uri copy = instance;
  ASSERT_EQ(instance, copy);
}

TEST(uri_test, assignment_test) {
  network::uri instance("http://www.example.com/");
  network::uri copy;
  copy = instance;
  ASSERT_EQ(instance, copy);
}

TEST(uri_test, swap_test) {
  network::uri instance("http://www.example.com/");
  network::uri copy("http://www.example.org/");
  network::swap(instance, copy);
  ASSERT_EQ(instance.string(), "http://www.example.org/");
  ASSERT_EQ(copy.string(), "http://www.example.com/");
}

TEST(uri_test, equality_test) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/");
  ASSERT_EQ(uri_1, uri_2);
}

TEST(uri_test, equality_test_1) {
  network::uri uri_1("http://www.example.com/");
  std::string uri_2("http://www.example.com/");
  ASSERT_EQ(uri_1, uri_2);
}

TEST(uri_test, equality_test_2) {
  std::string uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/");
  ASSERT_EQ(uri_1, uri_2);
}

//TEST(uri_test, equality_test_3) {
//  network::uri uri_1("http://www.example.com/");
//  std::string uri_2("http://www.example.com/");
//  ASSERT_TRUE(uri_1 == uri_2.c_str());
//}

//TEST(uri_test, equality_test_4) {
//  std::string uri_1("http://www.example.com/");
//  network::uri uri_2("http://www.example.com/");
//  ASSERT_TRUE(uri_1.c_str() == uri_2);
//}

TEST(uri_test, equality_test_reordered_query) {
  network::uri uri_1("http://www.example.com/?a=1&b=2");
  network::uri uri_2("http://www.example.com/?b=2&a=1");
  ASSERT_EQ(uri_1, uri_2);
}

TEST(uri_test, equality_test_capitalized_scheme) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("HTTP://www.example.com/");
  ASSERT_EQ(uri_1, uri_2);
}

TEST(uri_test, equality_test_capitalized_host) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://WWW.EXAMPLE.COM/");
  ASSERT_EQ(uri_1, uri_2);
}

TEST(uri_test, equality_test_user_info) {
  network::uri uri_1("ftp://john.doe@ftp.example.com/");
  network::uri uri_2("ftp://JOHN.DOE@ftp.example.com/");
  ASSERT_NE(uri_1, uri_2);
}

TEST(uri_test, equality_test_default_http_port) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com:80/");
  ASSERT_EQ(uri_1, uri_2);
}

TEST(uri_test, equality_test_default_http_port_2) {
  network::uri uri_1("http://www.example.com:80/");
  network::uri uri_2("http://www.example.com/");
  ASSERT_EQ(uri_1, uri_2);
}

TEST(uri_test, equality_test_default_https_port) {
  network::uri uri_1("https://www.example.com/");
  network::uri uri_2("https://www.example.com:443/");
  ASSERT_EQ(uri_1, uri_2);
}

TEST(uri_test, equality_test_default_https_port_2) {
  network::uri uri_1("https://www.example.com:443/");
  network::uri uri_2("https://www.example.com/");
  ASSERT_EQ(uri_1, uri_2);
}

//TEST(uri_test, equality_test_empty_path_with_trailing_slash) {
//  network::uri uri_1("http://www.example.com/");
//  network::uri uri_2("http://www.example.com");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_with_single_dot_segment) {
//  network::uri uri_1("http://www.example.com/./path");
//  network::uri uri_2("http://www.example.com/path");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_with_double_dot_segment) {
//  network::uri uri_1("http://www.example.com/1/../2/");
//  network::uri uri_2("http://www.example.com/2/");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_with_trailing_slash) {
//  network::uri uri_1("http://www.example.com/path/");
//  network::uri uri_2("http://www.example.com/path");
//  ASSERT_EQ(uri_1, uri_2);
//}

TEST(uri_test, equality_test_with_file_ext) {
  network::uri uri_1("http://www.example.com/filename.txt");
  network::uri uri_2("http://www.example.com/filename.txt/");
  ASSERT_NE(uri_1, uri_2);
}

TEST(uri_test, inequality_test) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/");
  ASSERT_TRUE(!(uri_1 != uri_2));
}

TEST(uri_test, less_than_test) {
  // uri_1 is lexicographically less than uri_2
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.org/");
  ASSERT_LT(uri_1, uri_2);
  //ASSERT_TRUE(uri_1 < uri_2);
}

//TEST(uri_test, username_test) {
//  network::uri instance("ftp://john.doe@ftp.example.com/");
//  ASSERT_EQ(network::username(instance), "john.doe");
//}
//
//TEST(uri_test, password_test) {
//  network::uri instance("ftp://john.doe:password@ftp.example.com/");
//  ASSERT_EQ(network::password(instance), "password");
//}

//TEST(uri_test, hierarchical_part_test) {
//  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
//  ASSERT_EQ(network::hierarchical_part(instance), "user:password@www.example.com:80/path");
//}

//TEST(uri_test, partial_hierarchical_part_test) {
//  network::uri instance("http://www.example.com?query#fragment");
//  ASSERT_EQ(network::hierarchical_part(instance), "www.example.com");
//}

//TEST(uri_test, authority_test) {
//  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
//  ASSERT_EQ(network::authority(instance), "user:password@www.example.com:80");
//}
//
//TEST(uri_test, partial_authority_test) {
//  network::uri instance("http://www.example.com/path?query#fragment");
//  ASSERT_EQ(network::authority(instance), "www.example.com");
//}
//
//TEST(uri_test, http_query_map_test) {
//  network::uri instance("http://user:password@www.example.com:80/path?query=something#fragment");
//
//  std::map<std::string, std::string> queries;
//  network::query_map(instance, queries);
//  BOOST_REQUIRE_EQUAL(queries.size(), std::size_t(1));
//  ASSERT_EQ(queries.begin()->first, "query");
//  ASSERT_EQ(queries.begin()->second, "something");
//}
//
//TEST(uri_test, xmpp_query_map_test) {
//  network::uri instance("xmpp:example-node@example.com?message;subject=Hello%20World");
//
//  std::map<std::string, std::string> queries;
//  network::query_map(instance, queries);
//  BOOST_REQUIRE_EQUAL(queries.size(), std::size_t(2));
//  ASSERT_EQ(queries.begin()->first, "message");
//  ASSERT_EQ(queries.begin()->second, "");
//  ASSERT_EQ((++queries.begin())->first, "subject");
//  ASSERT_EQ((++queries.begin())->second, "Hello%20World");
//}

TEST(uri_test, range_test) {
  const std::string url("http://www.example.com/");
  network::uri instance(url);
  ASSERT_TRUE(network::test::equal(instance, url));
}

TEST(uri_test, issue_67_test) {
  // https://github.com/cpp-netlib/cpp-netlib/issues/67
  const std::string site_name("http://www.google.com");
  network::uri bar0;
  network::uri bar1 = site_name;
  //ASSERT_TRUE_NO_THROW(bar0 = site_name);
}

TEST(uri_test, issue_104_test) {
  // https://github.com/cpp-netlib/cpp-netlib/issues/104
  std::unique_ptr<network::uri> instance(new network::uri("http://www.example.com/"));
  network::uri copy = *instance;
  instance.reset();
  ASSERT_EQ(copy.scheme().string(), "http");
}

TEST(uri_test, uri_set_test) {
  std::set<network::uri> uri_set;
  uri_set.insert(network::uri("http://www.example.com/"));
  ASSERT_FALSE(uri_set.empty());
  ASSERT_EQ((*uri_set.begin()), network::uri("http://www.example.com/"));
}

//TEST(uri_test, uri_unordered_set_test) {
//  std::unordered_set<network::uri> uri_set;
//  uri_set.insert(network::uri("http://www.example.com/"));
//  BOOST_REQUIRE(!uri_set.empty());
//  ASSERT_EQ((*uri_set.begin()), network::uri("http://www.example.com/"));
//}
