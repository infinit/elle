//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinit/hole/sdht/Sdht_loop.cc
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
    Sdht::look()
    {
      looker_ = new boost::thread(boost::bind(&Sdht::looking, this));
      
      return StatusOk;
    }


    Status
    Sdht::looking()
    {
      Natural(32)	time = 1;
      
      while (true)
	{	  
	  
	    if (ping_request() != StatusOk)
	    {
	      report.Display("Ping: ");
	      infinit_fail();
	    } 
	  
	    
	  if ((time % 30) == 0)
	    if ((stabilize()) != StatusOk)
	      {
		report.Display("Stabilize: ");
		infinit_fail();
	      }
	  
	  sleep(LOOK_TIME);
	  time++;
	}
      return StatusOk;
    }
    
  }
}
