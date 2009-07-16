//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinit/hole/sdht/Request.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//
#include <iostream>

#include "Request.hh"

#include <elle/misc/Status.hh>
#include <viewer/ubi/Viewer.hh>

using namespace elle;
using namespace viewer;
using namespace elle::misc;
using namespace elle::channel;

namespace hole
{
  namespace sdht
  { 
    Request::Request(const Node		&n,
		     const t_type	t)
      : port_(n.port_),
	ip_(n.ip_),
	type_(t)
    {
      
      id_ = random();
    }

    Request::Request()
      : port_(0),
	ip_("DTC")
    {
      id_ = -1;
      type_ = UNINIT;
    }
    
    Status
    Request::print() const
    {
      std::cout << *this << std::endl;
      infinit_leave();
    }
    
    Key
    Request::getKey() const
    {
      return (Key(ip_, port_));
    }
    
    std::ostream&	operator <<  (std::ostream	&s,
				      const Request	&r)
    {

      std::cout << "Request ";
      if (r.type_ == Request::PING)
      std::cout << "PING";
      if (r.type_ == Request::PONG)
      std::cout << "PONG";
      if (r.type_ == Request::LOOKUP)
      std::cout << "LOOKUP";
      if (r.type_ == Request::UNINIT)
      std::cout << "UNINIT";

      std::cout << " id: " << r.id_ << " from " << r.ip_;
      std::cout << ":" << r.port_ << " and data key: " << r.data_key_;  
      return s;
    }
    
  }
}
