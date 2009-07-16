//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinite/hole/sdht/Node.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//

#include "Node.hh"

#include <iostream>

#include <elle/misc/Status.hh>
#include <elle/misc/Network.hh>

using namespace elle;
using namespace elle::misc;

namespace hole
{
  namespace sdht
  {     
    Node::Node()
      : ip_("DTC"),
 	port_(0),
	key_(),
 	life_(NODE_LIFE)
    {
      throw "Should never be init herre";
    }
    
    Node::Node(const Ip			ip,
	       const Port		port)
      : ip_(ip),
	port_(port),
	key_(ip, port),
	life_(NODE_LIFE)
    {
      try
	{
	  chan_ = new Link_udpv4_sync(ip, port);
	}
      catch (...)
	{
	  std::cerr << "Exception while creating channel" << std::endl;
	}
    }

    Node::~Node()
    {
      delete chan_;
    }
    
    Status
    Node::print() const
    {
      std::cout << *this;
      return (StatusOk);
    }
    
    std::ostream&     
    operator<<		(std::ostream&	s,
			 const Node	&b)
    {
      s <<  b.ip_ << ":" << b.port_ << " <" << b.key_ << ">" << " life:" << b.life_;
      return s;
    }
    

  }
}
