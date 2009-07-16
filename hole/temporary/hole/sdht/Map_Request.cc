//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinit/hole/sdht/Map_Request.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//


#include "Map_Request.hh"

#include <list>
#include <elle/misc/Status.hh>

namespace hole
{
  namespace sdht
  { 
    Map_Request::Map_Request()
    {
    }

    Status
    Map_Request::add(Request					req)
    {
      r_[req.id_] = req;
      infinit_leave();
    }

    Status
    Map_Request::release(const Request::t_id			id)
    {
      std::map<Request::t_id, Request>::iterator		it;
      
      it = r_.find(id);
      if (it == r_.end())
	{
	  std::cout << "Cannot RELEASE an unknow Request id" << std::endl;
	  infinit_fail();
	}

      return StatusOk;
    }
    
    Status
    Map_Request::print()
    {
      std::map<Request::t_id, Request>::iterator		it;
      
      std::cout << "Map_Request list:" << std::endl;  
      if (r_.size() == 0)
	{
	  std::cout << "   Empty" << std::endl;
	  return StatusOk;
	}
      
      for (it = r_.begin(); it != r_.end(); it++)
	std::cout << "   " << it->second << std::endl;
      return StatusOk;
    }
  }
}
