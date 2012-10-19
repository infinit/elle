//              Copyright 2012 Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <network/uri/schemes.hpp>
#include <boost/unordered_map.hpp>

namespace network {
namespace {
static boost::unordered_map<std::string, std::string> hierarchical_schemes_;
static boost::unordered_map<std::string, std::string> opaque_schemes_;

bool register_hierarchical_schemes() {
    hierarchical_schemes_.insert(std::make_pair(std::string("http"), std::string("80")));
    hierarchical_schemes_.insert(std::make_pair(std::string("https"), std::string("443")));
    hierarchical_schemes_.insert(std::make_pair(std::string("shttp"), std::string("")));
    hierarchical_schemes_.insert(std::make_pair(std::string("ftp"), std::string("21")));
    hierarchical_schemes_.insert(std::make_pair(std::string("file"), std::string("")));
    hierarchical_schemes_.insert(std::make_pair(std::string("dns"), std::string("53")));
    hierarchical_schemes_.insert(std::make_pair(std::string("nfs"), std::string("2049")));
    hierarchical_schemes_.insert(std::make_pair(std::string("imap"), std::string("143")));
    hierarchical_schemes_.insert(std::make_pair(std::string("nntp"), std::string("")));
    hierarchical_schemes_.insert(std::make_pair(std::string("pop"), std::string("119")));
    hierarchical_schemes_.insert(std::make_pair(std::string("rsync"), std::string("873")));
    hierarchical_schemes_.insert(std::make_pair(std::string("snmp"), std::string("161")));
    hierarchical_schemes_.insert(std::make_pair(std::string("telnet"), std::string("23")));
    hierarchical_schemes_.insert(std::make_pair(std::string("svn"), std::string("3690")));
    hierarchical_schemes_.insert(std::make_pair(std::string("svn+ssh"), std::string("")));
    hierarchical_schemes_.insert(std::make_pair(std::string("git"), std::string("9418")));
    hierarchical_schemes_.insert(std::make_pair(std::string("git+ssh"), std::string("")));
    return true;
}

bool register_opaque_schemes() {
    opaque_schemes_.insert(std::make_pair(std::string("mailto"), std::string("25")));
    opaque_schemes_.insert(std::make_pair(std::string("sip"), std::string("5060")));
    opaque_schemes_.insert(std::make_pair(std::string("xmpp"), std::string("5222")));
    return true;
}


static bool hierarchical = register_hierarchical_schemes();
static bool opaque = register_opaque_schemes();
} // namespace

bool hierarchical_schemes::exists(const std::string &scheme) {
    return std::end(hierarchical_schemes_) != hierarchical_schemes_.find(scheme);
}

bool opaque_schemes::exists(const std::string &scheme) {
    return std::end(opaque_schemes_) != opaque_schemes_.find(scheme);
}

boost::optional<std::string> default_port(const std::string &scheme) {
	auto it = hierarchical_schemes_.find(scheme);
	if (it != std::end(hierarchical_schemes_)) {
		if (!it->second.empty()) {
			return it->second;
		}
	}

	it = opaque_schemes_.find(scheme);
	if (it != std::end(opaque_schemes_)) {
		if (!it->second.empty()) {
			return it->second;
		}
	}

	return boost::optional<std::string>();
}
} // namespace network
