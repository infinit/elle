// Copyright 2009-2010 Jeroen Habraken.
// Copyright 2009-2016 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint>
#include "../boost/config/warning_disable.hpp"
#include "../boost/spirit/home/qi.hpp"
#include "../boost/fusion/adapted/struct/adapt_struct.hpp"
#include "uri_parse_authority.hpp"

namespace {
struct authority_parts {
  network_boost::optional<network::uri::string_type> user_info, host, port;
};
} // namespace

BOOST_FUSION_ADAPT_STRUCT(
    authority_parts,
    (network_boost::optional<network::uri::string_type>, user_info)
    (network_boost::optional<network::uri::string_type>, host)
    (network_boost::optional<network::uri::string_type>, port));

namespace {
namespace qi = network_boost::spirit::qi;

struct authority_grammar
    : qi::grammar<typename network::uri::string_type::iterator, authority_parts()> {
  typedef network::uri::string_type string_type;
  typedef string_type::iterator iterator;

  authority_grammar() : authority_grammar::base_type(start, "authority") {
    // gen-delims = ":" / "/" / "?" / "#" / "[" / "]" / "@"
    gen_delims %= qi::char_(":/?#[]@");
    // sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," /
    // ";" / "="
    sub_delims %= qi::char_("!$&'()*+,;=");
    // reserved = gen-delims / sub-delims
    reserved %= gen_delims | sub_delims;
    // unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
    unreserved %= qi::alnum | qi::char_("-._~");
    // pct-encoded = "%" HEXDIG HEXDIG
    pct_encoded %= qi::char_("%") >> qi::repeat(2)[qi::xdigit];

    // user_info = *( unreserved / pct-encoded / sub-delims / ":" )
    user_info %=
        qi::raw[*(unreserved | pct_encoded | sub_delims | qi::char_(":"))];

    ip_literal %= qi::lit('[') >> (ipv6address | ipvfuture) >> ']';

    ipvfuture %=
        qi::lit('v') >> +qi::xdigit >> '.' >> +(unreserved | sub_delims | ':');

    ipv6addresses[0] %= qi::repeat(6)[h16 >> ':'] >> ls32;
    ipv6addresses[1] %= "::" >> qi::repeat(5)[h16 >> ':'] >> ls32;
    ipv6addresses[2] %= -qi::raw[h16] >> "::" >> qi::repeat(4)[h16 >> ':']
                                      >> ls32;
    ipv6addresses[3] %= -qi::raw[h16] >> "::" >> qi::repeat(3)[h16 >> ':']
                                      >> ls32;
    ipv6addresses[4] %= -qi::raw[h16] >> "::" >> qi::repeat(2)[h16 >> ':']
                                      >> ls32;
    ipv6addresses[5] %= -qi::raw[h16] >> "::" >> h16 >> ':' >> ls32;
    ipv6addresses[6] %= -qi::raw[h16] >> "::" >> ls32;
    ipv6addresses[7] %= -qi::raw[h16] >> "::" >> h16;
    ipv6addresses[8] %= -qi::raw[h16] >> "::";
    ipv6addresses[9] %= -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >>
      "::" >> qi::repeat(3)[h16 >> ':'] >> ls32;
    ipv6addresses[10] %= -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >>
      "::" >> qi::repeat(2)[h16 >> ':'] >> ls32;
    ipv6addresses[11] %= -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >>
      "::" >> h16 >> ':' >> ls32;
    ipv6addresses[12] %= -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >>
      "::" >> ls32;
    ipv6addresses[13] %= -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >>
      "::" >> h16;
    ipv6addresses[14] %= -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >>
      "::";
    ipv6addresses[15] %= -qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >>
      "::" >> qi::repeat(2)[h16 >> ':'] >> ls32;
    ipv6addresses[16] %= -qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >>
      "::" >> h16 >> ':' >> ls32;
    ipv6addresses[17] %= -qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >>
      "::" >> ls32;
    ipv6addresses[18] %= -qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >>
      "::" >> h16;
    ipv6addresses[19] %= -qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >>
      "::";
    ipv6addresses[20] %= -qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >>
      "::" >> h16 >> ':' >> ls32;
    ipv6addresses[21] %= -qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >>
      "::" >> ls32;
    ipv6addresses[22] %= -qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >>
      "::" >> h16;
    ipv6addresses[23] %= -qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >>
      "::";
    ipv6addresses[24] %= -qi::raw[qi::repeat(4)[(h16 >> ':')] >> h16] >>
      "::" >> ls32;
    ipv6addresses[25] %= -qi::raw[qi::repeat(4)[(h16 >> ':')] >> h16] >>
      "::" >> h16;
    ipv6addresses[26] %= -qi::raw[qi::repeat(4)[(h16 >> ':')] >> h16] >>
      "::";
    ipv6addresses[27] %= -qi::raw[qi::repeat(5)[(h16 >> ':')] >> h16] >>
      "::" >> h16;
    ipv6addresses[28] %= -qi::raw[qi::repeat(5)[(h16 >> ':')] >> h16] >>
      "::";
    ipv6addresses[29] %= -qi::raw[qi::repeat(6)[(h16 >> ':')] >> h16] >>
      "::";

    ipv6address %= qi::raw
      [ipv6addresses[0] |
       ipv6addresses[1] |
       ipv6addresses[2] |
       ipv6addresses[3] |
       ipv6addresses[4] |
       ipv6addresses[5] |
       ipv6addresses[6] |
       ipv6addresses[7] |
       ipv6addresses[8] |
       ipv6addresses[9] |
       ipv6addresses[10] |
       ipv6addresses[11] |
       ipv6addresses[12] |
       ipv6addresses[13] |
       ipv6addresses[14] |
       ipv6addresses[15] |
       ipv6addresses[16] |
       ipv6addresses[17] |
       ipv6addresses[18] |
       ipv6addresses[19] |
       ipv6addresses[20] |
       ipv6addresses[21] |
       ipv6addresses[22] |
       ipv6addresses[23] |
       ipv6addresses[24] |
       ipv6addresses[25] |
       ipv6addresses[26] |
       ipv6addresses[27] |
       ipv6addresses[28] |
       ipv6addresses[29]];

    // ls32 = ( h16 ":" h16 ) / IPv4address
    ls32 %= (h16 >> ':' >> h16) | ipv4address;

    // h16 = 1*4HEXDIG
    h16 %= qi::repeat(1, 4)[qi::xdigit];

    // dec-octet = DIGIT / %x31-39 DIGIT / "1" 2DIGIT / "2" %x30-34 DIGIT /
    // "25" %x30-35
    dec_octet %= !(qi::lit('0') >> qi::digit) >>
                 qi::raw[qi::uint_parser<std::uint8_t, 10, 1, 3>()];

    // IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
    ipv4address %=
        qi::raw[dec_octet >> qi::repeat(3)[qi::lit('.') >> dec_octet]];

    // reg-name = *( unreserved / pct-encoded / sub-delims )
    reg_name %= qi::raw[*(unreserved | pct_encoded | sub_delims)];

    // TODO, host = IP-literal / IPv4address / reg-name
    host %= qi::raw[ip_literal | ipv4address | reg_name];

    port %= qi::raw[qi::ushort_ | qi::eps];

    start %= (((user_info >> '@') | qi::eps) >> host >> -(':' >> port));
  }

  qi::rule<iterator, typename string_type::value_type()> gen_delims, sub_delims,
      reserved, unreserved;
  qi::rule<iterator, string_type()> pct_encoded;

  qi::rule<iterator, string_type()> dec_octet, ipv4address, reg_name,
      ipv6address, ipvfuture, ip_literal;

  qi::rule<iterator, string_type()> h16, ls32;
  qi::rule<iterator, string_type()> ipv6addresses[30];

  qi::rule<iterator, string_type()> user_info, host, port;

  // actual authority parser
  qi::rule<iterator, authority_parts()> start;
};
} // namespace

namespace network {
namespace detail {
bool parse_authority(uri::string_type &str,
                     network_boost::optional<uri::string_type> &user_info,
                     network_boost::optional<uri::string_type> &host,
                     network_boost::optional<uri::string_type> &port) {
  static authority_grammar grammar;
  authority_parts authority;
  auto first = std::begin(str), last = std::end(str);
  bool is_valid = qi::parse(first, last, grammar, authority);
  if (is_valid) {
    user_info = authority.user_info;
    host = authority.host;
    port = authority.port;
  }
  return is_valid && (first == last);
}
}  // namespace detail
}  // namespace network
