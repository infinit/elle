//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinite/hole/sdht/Sdht_default.cc
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
    Sdht::default_response(Request		&r)
    {
      Node		n(r.ip_, r.port_);
      
      m_node_.look(n);
      return StatusOk;
    }
    
   
  }
}
