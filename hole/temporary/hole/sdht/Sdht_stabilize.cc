//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinit/hole/sdht/Sdht_stabilize.cc
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

    /***
     ** @TODO don't forguet to stabilize more
     ** frequently the first successor
     **
     */
    Status
    Sdht::stabilize()
    {      
      std::map<Key, Node*>::iterator	it;
      
      for (it = m_node_.begin(); it != m_node_.end(); it++)
	{
	  lookup(it->first);
	}
      
      infinit_leave();
    }

    
   
  }
}
