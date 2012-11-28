// Copyright 2012 Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include <network/uri/uri.hpp>
#include <network/uri/detail/uri_parts.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/qi.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <algorithm>
#include <functional>
#include <map>

BOOST_FUSION_ADAPT_TPL_STRUCT
(
 (FwdIter),
 (network::detail::hierarchical_part)(FwdIter),
 (boost::optional<boost::iterator_range<FwdIter> >, user_info)
 (boost::optional<boost::iterator_range<FwdIter> >, host)
 (boost::optional<boost::iterator_range<FwdIter> >, port)
 (boost::optional<boost::iterator_range<FwdIter> >, path)
 );

BOOST_FUSION_ADAPT_TPL_STRUCT
(
 (FwdIter),
 (network::detail::uri_parts)(FwdIter),
 (boost::iterator_range<FwdIter>, scheme)
 (network::detail::hierarchical_part<FwdIter>, hier_part)
 (boost::optional<boost::iterator_range<FwdIter> >, query)
 (boost::optional<boost::iterator_range<FwdIter> >, fragment)
 );

namespace network {
  namespace qi = boost::spirit::qi;

  template <
    class String
    >
  struct uri_grammar : qi::grammar<
    typename String::const_iterator
    , detail::uri_parts<typename String::const_iterator>()> {

    typedef String string_type;
    typedef typename String::const_iterator const_iterator;

    uri_grammar() : uri_grammar::base_type(start, "uri") {
      // gen-delims = ":" / "/" / "?" / "#" / "[" / "]" / "@"
      gen_delims %= qi::char_(":/?#[]@");
      // sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
      sub_delims %= qi::char_("!$&'()*+,;=");
      // reserved = gen-delims / sub-delims
      reserved %= gen_delims | sub_delims;
      // unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
      unreserved %= qi::alnum | qi::char_("-._~");
      // pct-encoded = "%" HEXDIG HEXDIG
      pct_encoded %= qi::char_("%") >> qi::repeat(2)[qi::xdigit];

      // pchar = unreserved / pct-encoded / sub-delims / ":" / "@"
      pchar %= qi::raw[
		       unreserved | pct_encoded | sub_delims | qi::char_(":@")
		       ];

      // segment = *pchar
      segment %= qi::raw[*pchar];
      // segment-nz = 1*pchar
      segment_nz %= qi::raw[+pchar];
      // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
      segment_nz_nc %= qi::raw[
			       +(unreserved | pct_encoded | sub_delims | qi::char_("@"))
			       ];
      // path-abempty  = *( "/" segment )
      path_abempty %=
	qi::raw[*(qi::char_("/") >> segment)]
	;
      // path-absolute = "/" [ segment-nz *( "/" segment ) ]
      path_absolute %=
	qi::raw[
		qi::char_("/")
		>>  -(segment_nz >> *(qi::char_("/") >> segment))
		]
	;
      // path-rootless = segment-nz *( "/" segment )
      path_rootless %=
	qi::raw[segment_nz >> *(qi::char_("/") >> segment)]
	;
      // path-empty = 0<pchar>
      path_empty %=
	qi::raw[qi::eps]
	;

      // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
      scheme %=
	qi::raw[qi::alpha >> *(qi::alnum | qi::char_("+.-"))]
	;

      // user_info = *( unreserved / pct-encoded / sub-delims / ":" )
      user_info %=
	qi::raw[*(unreserved | pct_encoded | sub_delims | qi::char_(":"))]
	;

      ip_literal %=
	qi::lit('[') >> (ipv6address | ipvfuture) >> ']'
	;

      ipvfuture %=
	qi::lit('v') >> +qi::xdigit >> '.' >> +( unreserved | sub_delims | ':')
	;

      ipv6address %= qi::raw[
			     qi::repeat(6)[h16 >> ':'] >> ls32
			     |                                                  "::" >> qi::repeat(5)[h16 >> ':'] >> ls32
			     | - qi::raw[                               h16] >> "::" >> qi::repeat(4)[h16 >> ':'] >> ls32
			     | - qi::raw[                               h16] >> "::" >> qi::repeat(3)[h16 >> ':'] >> ls32
			     | - qi::raw[                               h16] >> "::" >> qi::repeat(2)[h16 >> ':'] >> ls32
			     | - qi::raw[                               h16] >> "::" >>               h16 >> ':'  >> ls32
			     | - qi::raw[                               h16] >> "::"                              >> ls32
			     | - qi::raw[                               h16] >> "::"                              >> h16
			     | - qi::raw[                               h16] >> "::"
			     | - qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::" >> qi::repeat(3)[h16 >> ':'] >> ls32
			     | - qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::" >> qi::repeat(2)[h16 >> ':'] >> ls32
			     | - qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::" >>               h16 >> ':'  >> ls32
			     | - qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::"                              >> ls32
			     | - qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::"                              >> h16
			     | - qi::raw[qi::repeat(1)[(h16 >> ':')] >> h16] >> "::"
			     | - qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >> "::" >> qi::repeat(2)[h16 >> ':'] >> ls32
			     | - qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >> "::" >>               h16 >> ':'  >> ls32
			     | - qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >> "::"                              >> ls32
			     | - qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >> "::"                              >> h16
			     | - qi::raw[qi::repeat(2)[(h16 >> ':')] >> h16] >> "::"
			     | - qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >> "::" >>               h16 >> ':'  >> ls32
			     | - qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >> "::"                              >> ls32
			     | - qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >> "::"                              >> h16
			     | - qi::raw[qi::repeat(3)[(h16 >> ':')] >> h16] >> "::"
			     | - qi::raw[qi::repeat(4)[(h16 >> ':')] >> h16] >> "::"                              >> ls32
			     | - qi::raw[qi::repeat(4)[(h16 >> ':')] >> h16] >> "::"                              >> h16
			     | - qi::raw[qi::repeat(4)[(h16 >> ':')] >> h16] >> "::"
			     | - qi::raw[qi::repeat(5)[(h16 >> ':')] >> h16] >> "::"                              >> h16
			     | - qi::raw[qi::repeat(5)[(h16 >> ':')] >> h16] >> "::"
			     | - qi::raw[qi::repeat(6)[(h16 >> ':')] >> h16] >> "::"
			     ];

      // ls32 = ( h16 ":" h16 ) / IPv4address
      ls32 %= (h16 >> ':' >> h16) | ipv4address
	;

      // h16 = 1*4HEXDIG
      h16 %= qi::repeat(1, 4)[qi::xdigit]
	;

      // dec-octet = DIGIT / %x31-39 DIGIT / "1" 2DIGIT / "2" %x30-34 DIGIT / "25" %x30-35
      dec_octet %=
	!(qi::lit('0') >> qi::digit)
	>>  qi::raw[
		    qi::uint_parser<boost::uint8_t, 10, 1, 3>()
		    ];

      // IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
      ipv4address %= qi::raw[
			     dec_octet >> qi::repeat(3)[qi::lit('.') >> dec_octet]
			     ];

      // reg-name = *( unreserved / pct-encoded / sub-delims )
      reg_name %= qi::raw[
			  *(unreserved | pct_encoded | sub_delims)
			  ];

      // TODO, host = IP-literal / IPv4address / reg-name
      host %=
	qi::raw[ip_literal | ipv4address | reg_name]
	;

      // port %= qi::ushort_;
      port %=
	qi::raw[*qi::digit]
	;

      // query = *( pchar / "/" / "?" )
      query %=
	qi::raw[*(pchar | qi::char_("/?"))]
	;

      // fragment = *( pchar / "/" / "?" )
      fragment %=
	qi::raw[*(pchar | qi::char_("/?"))]
	;

      // hier-part = "//" authority path-abempty / path-absolute / path-rootless / path-empty
      // authority = [ userinfo "@" ] host [ ":" port ]
      hier_part %=
	(
	 (("//" >> user_info >> '@') | "//")
	 >>  host
	 >> -(':' >> port)
	 >>  path_abempty
	 )
	|
	(
	 qi::attr(boost::iterator_range<const_iterator>())
	 >>  qi::attr(boost::iterator_range<const_iterator>())
	 >>  qi::attr(boost::iterator_range<const_iterator>())
	 >>  (
	      path_absolute
	      |   path_rootless
	      |   path_empty
	      )
	 )
	;

      start %=
	(scheme >> ':')
	>> hier_part
	>>  -('?' >> query)
	>>  -('#' >> fragment)
	;
    }

    qi::rule<const_iterator, typename boost::iterator_range<const_iterator>::value_type()>
    gen_delims, sub_delims, reserved, unreserved;
    qi::rule<const_iterator, string_type()>
    pct_encoded, pchar;

    qi::rule<const_iterator, string_type()>
    segment, segment_nz, segment_nz_nc;
    qi::rule<const_iterator, boost::iterator_range<const_iterator>()>
    path_abempty, path_absolute, path_rootless, path_empty;

    qi::rule<const_iterator, string_type()>
    dec_octet, ipv4address, reg_name, ipv6address, ipvfuture, ip_literal;

    qi::rule<const_iterator, string_type()>
    h16, ls32;

    qi::rule<const_iterator, boost::iterator_range<const_iterator>()>
    host, port;

    qi::rule<const_iterator, boost::iterator_range<const_iterator>()>
    scheme, user_info, query, fragment;

    qi::rule<const_iterator, detail::hierarchical_part<const_iterator>()>
    hier_part;

    // actual uri parser
    qi::rule<const_iterator, detail::uri_parts<const_iterator>()> start;

  };

  namespace detail {
    bool parse(std::string::const_iterator first,
	       std::string::const_iterator last,
	       uri_parts<std::string::const_iterator> &parts) {
      namespace qi = boost::spirit::qi;
      static uri_grammar<std::string> grammar;
      bool is_valid = qi::parse(first, last, grammar, parts);
      return is_valid && (first == last);
    }
  } // namespace detail

  uri::part_range::part_range() {

  }

  uri::part_range::part_range(const_iterator first, const_iterator last)
    : first_(first), last_(last) {

  }

  uri::part_range::const_iterator uri::part_range::begin() const {
    return first_;
  }

  uri::part_range::const_iterator uri::part_range::end() const {
    return last_;
  }

  bool uri::part_range::empty() const {
    return (first_ == last_);
  }

  uri::part_range::operator unspecified_bool_type () const {
    return !empty()? &part_range::empty : nullptr;
  }

  uri::part_range::string_type uri::part_range::native() const {
    return string_type(first_, last_);
  }

  std::string uri::part_range::string() const {
    return std::string(first_, last_);
  }

  std::wstring uri::part_range::wstring() const {
    return std::wstring(first_, last_);
  }

  std::u16string uri::part_range::u16string() const {
    return std::u16string(first_, last_);
  }

  std::u32string uri::part_range::u32string() const {
    return std::u32string(first_, last_);
  }

  uri::uri() {

  }

  uri::uri(const uri &other)
    : uri_(other.uri_) {
    parse();
  }

  uri::~uri() {

  }

  uri &uri::operator = (const uri &other) {
    uri_ = other.uri_;
    parse();
    return *this;
  }

  void uri::swap(uri &other) {
    std::swap(uri_, other.uri_);
    std::swap(uri_parts_, other.uri_parts_);
  }

  uri::const_iterator uri::begin() const {
    return uri_.begin();
  }

  uri::const_iterator uri::end() const {
    return uri_.end();
  }

  uri::part_range uri::scheme() const {
    return part_range(std::begin(uri_parts_.scheme), std::end(uri_parts_.scheme));
  }

  uri::part_range uri::user_info() const {
    return uri_parts_.hier_part.user_info?
      part_range(std::begin(uri_parts_.hier_part.user_info.get()),
		 std::end(uri_parts_.hier_part.user_info.get()))
      : part_range();
  }

  uri::part_range uri::host() const {
    return uri_parts_.hier_part.host?
      part_range(std::begin(uri_parts_.hier_part.host.get()),
		 std::end(uri_parts_.hier_part.host.get()))
      : part_range();
  }

  uri::part_range uri::port() const {
    return uri_parts_.hier_part.port?
      part_range(std::begin(uri_parts_.hier_part.port.get()),
		 std::end(uri_parts_.hier_part.port.get()))
      : part_range();
  }

  uri::part_range uri::path() const {
    return uri_parts_.hier_part.path?
      part_range(std::begin(uri_parts_.hier_part.path.get()),
		 std::end(uri_parts_.hier_part.path.get()))
      : part_range();
  }

  uri::part_range uri::query() const {
    return uri_parts_.query ?
      part_range(std::begin(uri_parts_.query.get()),
		 std::end(uri_parts_.query.get()))
      : part_range();
  }

  uri::part_range uri::fragment() const {
    return uri_parts_.fragment?
      part_range(std::begin(uri_parts_.fragment.get()),
		 std::end(uri_parts_.fragment.get()))
      : part_range();
  }

  uri::part_range uri::authority() const {
    return part_range(user_info().begin(), port().end());
  }


  uri::string_type uri::native() const {
    return uri_;
  }

  std::string uri::string() const {
    return std::string(std::begin(uri_), std::end(uri_));
  }

  std::wstring uri::wstring() const {
    return std::wstring(std::begin(uri_), std::end(uri_));
  }

  std::u16string uri::u16string() const {
    return std::u16string(std::begin(uri_), std::end(uri_));
  }

  std::u32string uri::u32string() const {
    return std::u32string(std::begin(uri_), std::end(uri_));
  }

  bool uri::empty() const {
    return uri_.empty();
  }

  bool uri::absolute() const {
    return !scheme().empty();
  }

  bool uri::opaque() const {
    return absolute();
  }

  uri uri::normalize() const {
    return *this;
  }

  uri uri::relativize(const uri &other) const {
    return *this;
  }

  uri uri::resolve(const uri &other) const {
    return *this;
  }

  void uri::parse() {
    if (uri_.empty()) {
      return;
    }

    const_iterator first(std::begin(uri_)), last(std::end(uri_));
    bool is_valid = detail::parse(first, last, uri_parts_);
    if (!is_valid) {
      throw uri_syntax_error("Unable to parse URI string.");
    }

    if (!uri_parts_.scheme) {
      uri_parts_.scheme = part_range(std::begin(uri_),
				     std::begin(uri_));
    }
    uri_parts_.update();
  }

  void swap(uri &lhs, uri &rhs) {
    lhs.swap(rhs);
  }

  namespace {
    template <class T>
    inline
    void hash_combine(std::size_t& seed, const T& v) {
      std::hash<T> hasher;
      seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
  } // namespace

  std::size_t hash_value(const uri &uri_) {
    std::size_t seed = 0;
    std::for_each(std::begin(uri_), std::end(uri_),
                  [&seed] (uri::value_type c) { hash_combine(seed, c); });
    return seed;
  }

  bool equals(const uri &lhs, const uri &rhs, uri_comparison_level level) {
    // if both URIs are empty, then we should define them as equal
    // even though they're still invalid.
    if (lhs.empty() && rhs.empty()) {
      return true;
    }

    if (lhs.empty() || rhs.empty()) {
      return false;
    }

    // the scheme can be compared insensitive to case
    bool equal = boost::iequals(lhs.scheme(), rhs.scheme());
    if (equal) {
      // the user info must be case sensitive
      equal = boost::equals(lhs.user_info(), rhs.user_info());
    }

    if (equal) {
      // the host can be compared insensitive to case
      equal = boost::iequals(lhs.host(), rhs.host());
    }

    if (equal) {
      if (lhs.port() && rhs.port()) {
    	equal = boost::equals(lhs.port(), rhs.port());
      }
     // else if (!lhs.port() && rhs.port()) {
    	//auto port = default_port(lhs.scheme());
    	//if (port) {
    	//  equal = boost::equals(*port, rhs.port());
    	//}
     // }
     // else if (lhs.port() && !rhs.port()) {
    	//auto port = default_port(rhs.scheme());
    	//if (port) {
    	//  equal = boost::equals(lhs.port(), *port);
    	//}
     // }
    }

    if (equal) {
      // test normalized paths
      //equal = boost::iequals(normalize_path(lhs.path()), normalize_path(rhs.path()));
      equal = boost::iequals(lhs.path(), rhs.path());
	}

    if (equal) {
      // test query, independent of order
      //std::map<uri::string_type, uri::string_type> lhs_query_params, rhs_query_params;
      //equal = (query_map(lhs, lhs_query_params) == query_map(rhs, rhs_query_params));
    }

    return equal;
  }

  bool operator == (const uri &lhs, const uri &rhs) {
    return equals(lhs, rhs, path_segment_normalization);
  }

  bool operator < (const uri &lhs, const uri &rhs) {
    return lhs.normalize().native() < rhs.normalize().native();
  }
} // namespace network
