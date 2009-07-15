//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinite/hole/sdht/Sdht_create.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//

#include "Sdht.hh"
#include "Request.hh"

#include <elle/misc/Status.hh>
#include <viewer/ubi/Viewer.hh>

#include <elle/channel/Channel.hh>
#include <elle/channel/Link_udpv4_sync.hh>

using namespace elle;
using namespace elle::misc;
using namespace elle::channel;
using namespace viewer;

namespace hole
{
  namespace sdht
  { 
    Status
    Sdht::join(const Ip		ip,
	       const Port	port)
    {   
   
      Viewer::add_node(me_.key_);
      this->listen();
      this->look();
      
      std::cout << "JOIN" << std::endl;
      Node n(ip, port);
      m_node_.look(n);
      Viewer::add_edge(me_.key_, Key(ip, port));



      



      
      return StatusOk;
    }
    
  }
}
