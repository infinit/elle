//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinit/hole/sdht/Sdht_ping.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//

#include "Sdht.hh"
#include "Request.hh"

#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>
#include <viewer/ubi/Viewer.hh>

using namespace elle;
using namespace elle::misc;
using namespace elle::channel;
using namespace viewer;

namespace hole
{
  namespace sdht
  { 
    Status
    Sdht::ping_request()
    {      
      Request				req(me_, Request::PING);
      std::map<Key, Node*>::iterator	it;
      
      for (it = m_node_.begin(); it != m_node_.end(); it++)
	{
	  if (it->second != NULL)
	    {
	      it->second->life_--;
	      if (it->second->life_ == 0)
		{
		  std::cout << "Deleting " << it->second->key_ << "for TIMEOUT." << std::endl;
		  Viewer::remove_edge(me_.key_, it->second->key_);
		  delete it->second;
		  it->second = NULL;
		}
	      else
		{
		  if (it->second->chan_->send(req) != StatusOk)
		    infinit_fail();
		}
	    }
	}
      infinit_leave();
    }

    Status
    Sdht::ping_response(Request		&r)
    {
      Channel		*chan = new Link_udpv4_sync(r.ip_, r.port_);
      Request		req(me_, Request::PONG);
      
      if (chan->send(req) != StatusOk)
	{
	  report.Display("Ping response");
	  delete chan;
	  infinit_fail();
	}
      delete chan;
      return StatusOk;
    }

    Status
    Sdht::pong_response(Request		&r)
    {
      m_node_.touch(r.getKey());
      return StatusOk;
    }


   
  }
}
