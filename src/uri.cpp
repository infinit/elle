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

namespace network {
#if defined(BOOST_NO_CXX11_NOEXCEPT)
  const char *uri_category_impl::name() const {
#else
  const char *uri_category_impl::name() const noexcept {
#endif // defined(BOOST_NO_CXX11_NOEXCEPT)
    return "uri_error";
  }

  std::string uri_category_impl::message(int ev) const {
    switch (uri_error(ev)) {
    case uri_error::invalid_syntax:
      return "Unable to parse URI string.";
    default:
      break;
    }
    return std::string("Unknown URI error.");
  }

  const std::error_category &uri_category() {
    static uri_category_impl uri_category;
    return uri_category;
  }

  std::error_code make_error_code(uri_error e) {
    return std::error_code(static_cast<int>(e), uri_category());
  }
} // namespace network

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

  uri::uri(const boost::optional<string_type> &scheme,
	   const boost::optional<string_type> &user_info,
	   const boost::optional<string_type> &host,
	   const boost::optional<string_type> &port,
	   const boost::optional<string_type> &path,
	   const boost::optional<string_type> &query,
	   const boost::optional<string_type> &fragment) {

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
      }
      else {
	auto ec = make_error_code(uri_error::invalid_host);
	throw std::system_error(ec);
      }

      if (port) {
	uri_.append(":");
	uri_.append(*port);
      }
    }
    else {
      if (scheme) {
	if (path || query || fragment) {
	  uri_.append(":");
	}
	else {
	  auto ec = make_error_code(uri_error::invalid_scheme);
	  throw std::system_error(ec);
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
      //uri_parts_.scheme = boost::iterator_range<const_iterator>(std::begin(uri_), std::end(uri_));
    }

    if (user_info) {

    }

    if (host) {

    }

    if (port) {

    }

    if (path) {

    }

    if (query) {

    }

    if (fragment) {

    }

    std::error_code ec;
    parse(ec);
  }

  uri::uri() {

  }

  uri::uri(const uri &other)
    : uri_(other.uri_) {
    std::error_code ec;
    parse(ec);
    // don't throw
  }

  uri::~uri() {

  }

  uri &uri::operator = (const uri &other) {
    uri_ = other.uri_;
    std::error_code ec;
    parse(ec);
    // don't throw
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

  boost::optional<string_ref> uri::scheme() const {
    return string_ref(std::begin(uri_parts_.scheme), std::end(uri_parts_.scheme));
  }

  boost::optional<string_ref> uri::user_info() const {
    return uri_parts_.hier_part.user_info?
      string_ref(std::begin(uri_parts_.hier_part.user_info.get()),
		 std::end(uri_parts_.hier_part.user_info.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::host() const {
    return uri_parts_.hier_part.host?
      string_ref(std::begin(uri_parts_.hier_part.host.get()),
		 std::end(uri_parts_.hier_part.host.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::port() const {
    return uri_parts_.hier_part.port?
      string_ref(std::begin(uri_parts_.hier_part.port.get()),
		 std::end(uri_parts_.hier_part.port.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::path() const {
    return uri_parts_.hier_part.path?
      string_ref(std::begin(uri_parts_.hier_part.path.get()),
		 std::end(uri_parts_.hier_part.path.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::query() const {
    return uri_parts_.query ?
      string_ref(std::begin(uri_parts_.query.get()),
		 std::end(uri_parts_.query.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::fragment() const {
    return uri_parts_.fragment?
      string_ref(std::begin(uri_parts_.fragment.get()),
		 std::end(uri_parts_.fragment.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::authority() const {
    auto host = this->host();
    if (!host) {
      return boost::optional<string_ref>();
    }

    auto first = std::begin(*host), last = std::end(*host);
    if (user_info()) {
      first = std::begin(*user_info());
    }

    if (port()) {
      last = std::end(*port());
    }

    return string_ref(first, last);
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
    return static_cast<bool>(scheme());
  }

  bool uri::opaque() const {
    return (absolute() && !authority());
  }

  uri uri::normalize(uri_comparison_level level) const {

    string_type normalized(uri_);

    if (uri_comparison_level::case_normalization == level) {
      // All alphabetic characters are lower-case except when used in
      // percent encoding
      auto it = std::begin(normalized);
      while (it != std::end(normalized)) {
	if (*it == '%') {
	  ++it; *it = std::toupper(*it);
	  ++it; *it = std::toupper(*it);
	}
	else {
	  *it = std::tolower(*it);
	}
	++it;
      }
    }

    if (uri_comparison_level::percent_encoding_normalization == level) {
      // alpha range %41-%5A, %61-%7A

      // digit %30-%39

      // hyphen %2D

      // period %2E

      // underscore %5F

      // tilde %7E
    }

    if (uri_comparison_level::path_segment_normalization == level) {

    }

    return uri(normalized);
  }

  //uri uri::relativize(const uri &other) const {
  //  if (opaque() || other.opaque()) {
  //    return other;
  //  }
  //
  //  return other;
  //}
  //
  //uri uri::resolve(const uri &other) const {
  //  // http://tools.ietf.org/html/rfc3986#section-5.2
  //
  //
  //  //if (!other.absolute() && !other.opaque()) {
  //  //
  //  //}
  //  return other;
  //}

  void uri::parse(std::error_code &ec) {
    if (uri_.empty()) {
      return;
    }

    const_iterator first(std::begin(uri_)), last(std::end(uri_));
    bool is_valid = detail::parse(first, last, uri_parts_);
    if (!is_valid) {
      ec = make_error_code(uri_error::invalid_syntax);
      return;
    }

    if (!uri_parts_.scheme) {
      uri_parts_.scheme = string_ref(std::begin(uri_),
				     std::begin(uri_));
    }
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

    return lhs.normalize(level).native() == rhs.normalize(level).native();
  }
} // namespace network
