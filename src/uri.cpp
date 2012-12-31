// Copyright 2012 Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <network/uri/uri.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/qi.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/optional.hpp>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>

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

  namespace detail {
    template <
      class FwdIter
      >
    struct hierarchical_part {
      boost::optional<boost::iterator_range<FwdIter> > user_info;
      boost::optional<boost::iterator_range<FwdIter> > host;
      boost::optional<boost::iterator_range<FwdIter> > port;
      boost::optional<boost::iterator_range<FwdIter> > path;
    };

    template <
      class FwdIter
      >
    struct uri_parts {
      boost::optional<boost::iterator_range<FwdIter> > scheme;
      hierarchical_part<FwdIter> hier_part;
      boost::optional<boost::iterator_range<FwdIter> > query;
      boost::optional<boost::iterator_range<FwdIter> > fragment;
    };
  } // namespace detail
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
 (boost::optional<boost::iterator_range<FwdIter> >, scheme)
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
    typename String::iterator
    , detail::uri_parts<typename String::iterator>()> {

    typedef String string_type;
    typedef typename String::iterator iterator;

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
	 qi::attr(boost::optional<boost::iterator_range<iterator> >())
	 >>  qi::attr(boost::optional<boost::iterator_range<iterator> >())
	 >>  qi::attr(boost::optional<boost::iterator_range<iterator> >())
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

    qi::rule<iterator, typename boost::iterator_range<iterator>::value_type()>
    gen_delims, sub_delims, reserved, unreserved;
    qi::rule<iterator, string_type()>
    pct_encoded, pchar;

    qi::rule<iterator, string_type()>
    segment, segment_nz, segment_nz_nc;
    qi::rule<iterator, boost::iterator_range<iterator>()>
    path_abempty, path_absolute, path_rootless, path_empty;

    qi::rule<iterator, string_type()>
    dec_octet, ipv4address, reg_name, ipv6address, ipvfuture, ip_literal;

    qi::rule<iterator, string_type()>
    h16, ls32;

    qi::rule<iterator, boost::iterator_range<iterator>()>
    host, port;

    qi::rule<iterator, boost::iterator_range<iterator>()>
    scheme, user_info, query, fragment;

    qi::rule<iterator, detail::hierarchical_part<iterator>()>
    hier_part;

    // actual uri parser
    qi::rule<iterator, detail::uri_parts<iterator>()> start;

  };

  namespace detail {
    bool parse(uri::string_type::iterator first,
	       uri::string_type::iterator last,
	       uri_parts<uri::string_type::iterator> &parts) {
      namespace qi = boost::spirit::qi;
      static uri_grammar<uri::string_type> grammar;
      bool is_valid = qi::parse(first, last, grammar, parts);
      return is_valid && (first == last);
    }
  } // namespace detail

  struct uri::impl {

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif // defined(_MSC_VER)

    string_type uri_;

#if defined(_MSC_VER)
#pragma warning(push)
#endif // defined(_MSC_VER)

    detail::uri_parts<string_type::iterator> uri_parts_;

    impl *clone();

  };

  uri::impl *uri::impl::clone() {
    impl *other = new impl;
    other->uri_ = uri_;
    if (!other->uri_.empty()) {
      auto first = std::begin(other->uri_), last = std::end(other->uri_);
      bool is_valid = detail::parse(first, last, other->uri_parts_);
      assert(is_valid);
    }
    return other;
  }

  uri::uri(const boost::optional<string_type> &scheme,
	   const boost::optional<string_type> &user_info,
	   const boost::optional<string_type> &host,
	   const boost::optional<string_type> &port,
	   const boost::optional<string_type> &path,
	   const boost::optional<string_type> &query,
	   const boost::optional<string_type> &fragment) {
    string_type uri;

    if (scheme) {
      uri.append(*scheme);
    }

    if (user_info || host || port) {
      if (scheme) {
	uri.append("://");
      }

      if (user_info) {
	uri.append(*user_info);
	uri.append("@");
      }

      if (host) {
	uri.append(*host);
      }
      else {
	auto ec = make_error_code(uri_error::invalid_host);
	throw std::system_error(ec);
      }

      if (port) {
	uri.append(":");
	uri.append(*port);
      }
    }
    else {
      if (scheme) {
	if (path || query || fragment) {
	  uri.append(":");
	}
	else {
	  auto ec = make_error_code(uri_error::invalid_scheme);
	  throw std::system_error(ec);
	}
      }
    }

    if (path) {
      uri.append(*path);
    }

    if (query) {
      uri.append("?");
      uri.append(*query);
    }

    if (fragment) {
      uri.append("#");
      uri.append(*fragment);
    }

    auto it = std::begin(uri);
    if (scheme) {
      //uri_parts_.scheme = boost::iterator_range<iterator>(std::begin(uri_), std::end(uri_));
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
    init(uri, ec);
    if (ec) {
      throw std::system_error(ec);
    }
  }

  uri::uri()
    : pimpl_(new impl) {

  }

  uri::uri(const uri &other)
    : pimpl_(other.pimpl_->clone()) {

  }

  uri::uri(uri &&other)
    : pimpl_(other.pimpl_) {
    other.pimpl_ = new impl;
  }

  uri::~uri() {
    delete pimpl_;
  }

  uri &uri::operator = (const uri &other) {
    uri(other).swap(*this);
    return *this;
  }

  uri &uri::operator = (uri &&other) {
    uri(other).swap(*this);
    return *this;
  }

  void uri::swap(uri &other) {
    std::swap(pimpl_, other.pimpl_);
  }

  uri::const_iterator uri::begin() const {
    return pimpl_->uri_.begin();
  }

  uri::const_iterator uri::end() const {
    return pimpl_->uri_.end();
  }

  boost::optional<string_ref> uri::scheme() const {
    return pimpl_->uri_parts_.scheme?
      string_ref(std::begin(pimpl_->uri_parts_.scheme.get()), std::end(pimpl_->uri_parts_.scheme.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::user_info() const {
    return pimpl_->uri_parts_.hier_part.user_info?
      string_ref(std::begin(pimpl_->uri_parts_.hier_part.user_info.get()),
		 std::end(pimpl_->uri_parts_.hier_part.user_info.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::host() const {
    return pimpl_->uri_parts_.hier_part.host?
      string_ref(std::begin(pimpl_->uri_parts_.hier_part.host.get()),
		 std::end(pimpl_->uri_parts_.hier_part.host.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::port() const {
    return pimpl_->uri_parts_.hier_part.port?
      string_ref(std::begin(pimpl_->uri_parts_.hier_part.port.get()),
		 std::end(pimpl_->uri_parts_.hier_part.port.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::path() const {
    return pimpl_->uri_parts_.hier_part.path?
      string_ref(std::begin(pimpl_->uri_parts_.hier_part.path.get()),
		 std::end(pimpl_->uri_parts_.hier_part.path.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::query() const {
    return pimpl_->uri_parts_.query ?
      string_ref(std::begin(pimpl_->uri_parts_.query.get()),
		 std::end(pimpl_->uri_parts_.query.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::fragment() const {
    return pimpl_->uri_parts_.fragment?
      string_ref(std::begin(pimpl_->uri_parts_.fragment.get()),
		 std::end(pimpl_->uri_parts_.fragment.get()))
      : boost::optional<string_ref>();
  }

  boost::optional<string_ref> uri::authority() const {
    auto host = this->host();
    if (!host) {
      return boost::optional<string_ref>();
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

    return string_ref(first, last);
  }

  uri::string_type uri::native() const {
    return pimpl_->uri_;
  }

  std::string uri::string() const {
    return std::string(std::begin(pimpl_->uri_), std::end(pimpl_->uri_));
  }

  std::wstring uri::wstring() const {
    return std::wstring(std::begin(pimpl_->uri_), std::end(pimpl_->uri_));
  }

  std::u16string uri::u16string() const {
    return std::u16string(std::begin(pimpl_->uri_), std::end(pimpl_->uri_));
  }

  std::u32string uri::u32string() const {
    return std::u32string(std::begin(pimpl_->uri_), std::end(pimpl_->uri_));
  }

  bool uri::empty() const {
    return pimpl_->uri_.empty();
  }

  bool uri::absolute() const {
    return static_cast<bool>(scheme());
  }

  bool uri::opaque() const {
    return (absolute() && !authority());
  }

  namespace {
    std::unordered_map<std::string, char> make_percent_decoded_chars() {
      std::unordered_map<std::string, char> map;
      map["%41"] = 'a'; map["%61"] = 'A';
      map["%42"] = 'b'; map["%62"] = 'B';
      map["%43"] = 'c'; map["%63"] = 'C';
      map["%44"] = 'd'; map["%64"] = 'D';
      map["%45"] = 'e'; map["%65"] = 'E';
      map["%46"] = 'f'; map["%66"] = 'F';
      map["%47"] = 'g'; map["%67"] = 'G';
      map["%48"] = 'h'; map["%68"] = 'H';
      map["%49"] = 'i'; map["%69"] = 'I';
      map["%4A"] = 'j'; map["%6A"] = 'J';
      map["%4B"] = 'k'; map["%6B"] = 'K';
      map["%4C"] = 'l'; map["%6C"] = 'L';
      map["%4D"] = 'm'; map["%6D"] = 'M';
      map["%4E"] = 'n'; map["%6E"] = 'N';
      map["%4F"] = 'o'; map["%6F"] = 'O';
      map["%50"] = 'p'; map["%70"] = 'P';
      map["%51"] = 'q'; map["%71"] = 'Q';
      map["%52"] = 'r'; map["%72"] = 'R';
      map["%53"] = 's'; map["%73"] = 'S';
      map["%54"] = 't'; map["%74"] = 'T';
      map["%55"] = 'u'; map["%75"] = 'U';
      map["%56"] = 'v'; map["%76"] = 'V';
      map["%57"] = 'w'; map["%77"] = 'W';
      map["%58"] = 'x'; map["%78"] = 'X';
      map["%59"] = 'y'; map["%79"] = 'Y';
      map["%5A"] = 'z'; map["%7A"] = 'Z';
      map["%30"] = '0';
      map["%31"] = '1';
      map["%32"] = '2';
      map["%33"] = '3';
      map["%34"] = '4';
      map["%35"] = '5';
      map["%36"] = '6';
      map["%37"] = '7';
      map["%38"] = '8';
      map["%39"] = '9';
      map["%2D"] = '-';
      map["%2E"] = '.';
      map["%5F"] = '_';
      map["%7E"] = '~';
      return map;
    }

    static const std::unordered_map<std::string, char> percent_decoded_chars =
      make_percent_decoded_chars();
  } // namespace

  uri uri::normalize(uri_comparison_level level) const {
    if ((uri_comparison_level::case_normalization == level) ||
	(uri_comparison_level::percent_encoding_normalization == level) ||
	(uri_comparison_level::path_segment_normalization == level)) {
      // All alphabetic characters in the scheme and host are
      // lower-case...
      if (pimpl_->uri_parts_.scheme) {
	boost::to_lower(*pimpl_->uri_parts_.scheme);
      }

      if (pimpl_->uri_parts_.hier_part.host) {
	boost::to_lower(*pimpl_->uri_parts_.hier_part.host);
      }

      // ...except when used in percent encoding
      auto it = std::begin(pimpl_->uri_), end = std::end(pimpl_->uri_);
      while (it != end) {
	if (*it == '%') {
	  ++it; *it = std::toupper(*it);
	  ++it; *it = std::toupper(*it);
	}
	++it;
      }
    }

    string_type normalized(pimpl_->uri_);
    if ((uri_comparison_level::percent_encoding_normalization == level) ||
	(uri_comparison_level::path_segment_normalization == level)) {
      auto it = std::begin(normalized), it2 = std::begin(normalized), end = std::end(normalized);
      while (it != end) {
	if (*it == '%') {
	  auto sfirst = it, slast = it;
	  std::advance(slast, 3);
	  auto char_it = percent_decoded_chars.find(string_type(sfirst, slast));
	  if (char_it != std::end(percent_decoded_chars)) {
	    *it2 = char_it->second;
	  }
	  ++it; ++it;
	}
	else {
	  *it2 = *it;
	}
	++it; ++it2;
      }
      normalized.erase(it2, end);
    }

    if (uri_comparison_level::path_segment_normalization == level) {
      auto first = std::begin(normalized), last = std::end(normalized);
      detail::uri_parts<string_type::iterator> parts;
      bool is_valid = detail::parse(first, last, parts);
      assert(is_valid);

      if (parts.hier_part.path) {
	using namespace boost;
	using namespace boost::algorithm;

	string_type path(std::begin(*parts.hier_part.path), std::end(*parts.hier_part.path));
	boost::optional<string_type> query, fragment;
	if (parts.query) {
	  query = string_type(std::begin(*parts.query), std::end(*parts.query));
	}

	if (parts.fragment) {
	  fragment = string_type(std::begin(*parts.fragment), std::end(*parts.fragment));
	}

	if (path.empty()) {
	  path = "/";
	}
	else {
	  std::vector<string_type> path_segments;
	  boost::split(path_segments, path, is_any_of("/"));

	  // remove single dot segments
	  boost::remove_erase_if(path_segments,
				 [] (const uri::string_type &s) {
				   return boost::equal(s, as_literal("."));
				 });

	  // remove double dot segments
	  std::vector<string_type> normalized_segments;
	  auto depth = 0;
	  boost::for_each(path_segments,
			  [&normalized_segments, &depth] (const string_type &s) {
			    assert(depth >= 0);
			    if (boost::equal(s, as_literal(".."))) {
			      normalized_segments.pop_back();
			    }
			    else {
			      normalized_segments.push_back(s);
			    }
			  });

	  path = boost::join(normalized_segments, "/");
	}

	string_type::iterator path_begin = std::begin(normalized);
	std::advance(path_begin,
		     std::distance<string_type::iterator>(path_begin,
							  std::begin(*parts.hier_part.path)));
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

  //uri uri::relativize(const uri &other, uri_comparison_level level) const {
  //  if (opaque() || other.opaque()) {
  //    return other;
  //  }
  //
  //  if ((scheme() && other.scheme()) &&
  //	(scheme().get() == other.scheme().get())) {
  //    return other;
  //  }
  //
  //  return other;
  //}

  //uri uri::resolve(const uri &other, uri_comparison_level level) const {
  //  // http://tools.ietf.org/html/rfc3986#section-5.2
  //
  //
  //  //if (!other.absolute() && !other.opaque()) {
  //  //
  //  //}
  //  return other;
  //}

  int uri::compare(const uri &other, uri_comparison_level level) const {
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

    return normalize(level).native().compare(other.normalize(level).native());
  }

  void uri::init(const string_type &uri, std::error_code &ec) {
    pimpl_ = new impl;

    pimpl_->uri_ = boost::trim_copy(uri);
    if (!pimpl_->uri_.empty()) {
      auto first = std::begin(pimpl_->uri_), last = std::end(pimpl_->uri_);
      bool is_valid = detail::parse(first, last, pimpl_->uri_parts_);
      if (!is_valid) {
	ec = make_error_code(uri_error::invalid_syntax);
      }
    }
  }
} // namespace network
