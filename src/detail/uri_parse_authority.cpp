// Copyright 2009-2010 Jeroen Habraken.
// Copyright 2009-2013 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/qi.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include "detail/uri_parse_authority.hpp"

namespace network {
  namespace detail {
    struct authority_parts {

      boost::optional<uri::string_type> user_info, host, port;

    };
  } // namespace detail
} // namespace network

BOOST_FUSION_ADAPT_STRUCT(
    network::detail::authority_parts,
    (boost::optional<network::uri::string_type>, user_info)
    (boost::optional<network::uri::string_type>, host)
    (boost::optional<network::uri::string_type>, port));

namespace network {
  namespace detail {
    namespace qi = boost::spirit::qi;

    template <class String>
    struct authority_grammar
        : qi::grammar<typename String::iterator,
                      authority_parts()> {

      typedef String string_type;
      typedef typename String::iterator iterator;

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

        ipvfuture %= qi::lit('v') >> +qi::xdigit >> '.' >>
                     +(unreserved | sub_delims | ':');

        ipv6address %= qi::raw
            [qi::repeat(6)[h16 >> ':'] >> ls32 |
             "::" >> qi::repeat(5)[h16 >> ':'] >> ls32 |
             -qi::raw[h16] >> "::" >> qi::repeat(4)[h16 >> ':'] >> ls32 |
             -qi::raw[h16] >> "::" >> qi::repeat(3)[h16 >> ':'] >> ls32 |
             -qi::raw[h16] >> "::" >> qi::repeat(2)[h16 >> ':'] >> ls32 |
             -qi::raw[h16] >> "::" >> h16 >> ':' >> ls32 |
             -qi::raw[h16] >> "::" >> ls32 | -qi::raw[h16] >> "::" >> h16 |
             -qi::raw[h16] >> "::" |
             -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::" >>
                 qi::repeat(3)[h16 >> ':'] >> ls32 |
             -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::" >>
                 qi::repeat(2)[h16 >> ':'] >> ls32 |
             -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::" >> h16 >>
                 ':' >> ls32 |
             -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::" >> ls32 |
             -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::" >> h16 |
             -qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::" |
             -qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >> "::" >>
                 qi::repeat(2)[h16 >> ':'] >> ls32 |
             -qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >> "::" >> h16 >>
                 ':' >> ls32 |
             -qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >> "::" >> ls32 |
             -qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >> "::" >> h16 |
             -qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >> "::" |
             -qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >> "::" >> h16 >>
                 ':' >> ls32 |
             -qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >> "::" >> ls32 |
             -qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >> "::" >> h16 |
             -qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >> "::" |
             -qi::raw[qi::repeat(4)[(h16 >> ':')] >> h16] >> "::" >> ls32 |
             -qi::raw[qi::repeat(4)[(h16 >> ':')] >> h16] >> "::" >> h16 |
             -qi::raw[qi::repeat(4)[(h16 >> ':')] >> h16] >> "::" |
             -qi::raw[qi::repeat(5)[(h16 >> ':')] >> h16] >> "::" >> h16 |
             -qi::raw[qi::repeat(5)[(h16 >> ':')] >> h16] >> "::" |
             -qi::raw[qi::repeat(6)[(h16 >> ':')] >> h16] >> "::"];

        // ls32 = ( h16 ":" h16 ) / IPv4address
        ls32 %= (h16 >> ':' >> h16) | ipv4address;

        // h16 = 1*4HEXDIG
        h16 %= qi::repeat(1, 4)[qi::xdigit];

        // dec-octet = DIGIT / %x31-39 DIGIT / "1" 2DIGIT / "2" %x30-34 DIGIT /
        // "25" %x30-35
        dec_octet %= !(qi::lit('0') >> qi::digit) >>
                     qi::raw[qi::uint_parser<boost::uint8_t, 10, 1, 3>()];

        // IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
        ipv4address %=
            qi::raw[dec_octet >> qi::repeat(3)[qi::lit('.') >> dec_octet]];

        // reg-name = *( unreserved / pct-encoded / sub-delims )
        reg_name %= qi::raw[*(unreserved | pct_encoded | sub_delims)];

        // TODO, host = IP-literal / IPv4address / reg-name
        host %= qi::raw[ip_literal | ipv4address | reg_name];

        port %= qi::raw[qi::ushort_ | qi::eps];

        start %=
          (((user_info >> '@') | qi::eps) >> host >> -(':' >> port));
      }

      qi::rule<iterator, typename boost::iterator_range<iterator>::value_type()>
          gen_delims, sub_delims, reserved, unreserved;
      qi::rule<iterator, string_type()> pct_encoded;

      qi::rule<iterator, string_type()> dec_octet, ipv4address, reg_name,
          ipv6address, ipvfuture, ip_literal;

      qi::rule<iterator, string_type()> h16, ls32;

      qi::rule<iterator, string_type()> user_info, host, port;

      // actual authority parser
      qi::rule<iterator, detail::authority_parts()> start;
    };

    bool parse_authority(uri::string_type::iterator first,
                         uri::string_type::iterator last,
                         boost::optional<uri::string_type> &user_info,
                         boost::optional<uri::string_type> &host,
                         boost::optional<uri::string_type> &port) {
      namespace qi = boost::spirit::qi;
      static authority_grammar<uri::string_type> grammar;
      authority_parts authority;
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
