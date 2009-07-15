//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinite/hole/sdht/Sdht_listen.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//

#include <boost/thread/thread.hpp>

#include "Sdht.hh"
#include "Request.hh"

#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>
#include <elle/channel/Channel.hh>
#include <elle/channel/Link_udpv4_sync.hh>


using namespace elle;
using namespace elle::misc;
using namespace elle::channel;

namespace hole
{
  namespace sdht
  { 
    Status
    Sdht::listen()
    {
      listener_ = new boost::thread(boost::bind(&Sdht::listening, this));
      
      return StatusOk;
    }

    Status
    Sdht::listening()
    {
      Channel		*chan = new Link_udpv4_sync(me_.port_);
      Request		req;
      
      while (true)
	{
	  if (chan->receive<Request>(req) != StatusOk)
	    {
	      report.Display("Listen: ");
	      break;
	    }
	  
	  std::cout << "RESCIVE: "<< req << std::endl;
	  
	  switch (req.type_)
	    {
	    case Request::UNINIT:
	      std::cerr << "UNINIT Request" << std::endl; 
	      break;
	    case Request::PING:
	      ping_response(req);
	      break;
	    case Request::PONG:
	      pong_response(req);
	      break;
	    case Request::LOOKUP:
	      lookup_response(req);
	      break;
	    default:
	      std::cerr << "IMPOSSIBLE UNKNOW REQUEST!!!!!" << std::endl;
	      break;
	    }
	  
	  default_response(req);	  
	}
      
      delete chan;
      return StatusOk;
    }
    
   
  }
}
