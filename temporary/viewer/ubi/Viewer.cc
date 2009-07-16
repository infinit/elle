//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinit/hole/sdht/Sdht.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//

#include "Viewer.hh"

#include <iostream>

#include <elle/misc/Natural.hh>
#include <hole/Key.hh>

using namespace elle;
using namespace elle::misc;
using namespace hole;


namespace viewer
{

  void
  Viewer::init()
  {
#ifdef VIEWER
    std::cout << "Starting DHT graph viewer" << std::endl;
#endif     
  }
  
  void
  Viewer::clear()
  {
#ifdef VIEWER
    ubigraph_clear();
#endif 
  }
  
  void 
  Viewer::add_node(const Key	&k)
  {    
    Natural(16)		vertex_id = k.to_natural16();

    std::cout << "VIEWER: Adding key: 0x" << std::hex << vertex_id << std::endl;
#ifdef VIEWER
    ubigraph_new_vertex_w_id(vertex_id);
    ubigraph_set_vertex_attribute(vertex_id, "label", k.to_string().c_str());
    ubigraph_set_vertex_attribute(vertex_id, "shape", "sphere");
#endif
  }
  
  void 
  Viewer::remove_node(const Key	&k)
  {    
    Natural(16)		vertex_id = k.to_natural16();

    std::cout << "VIEWER: Removing key: 0x" << std::hex << vertex_id << std::endl;
  #ifdef VIEWER
    ubigraph_remove_vertex(vertex_id);
  #endif
  }
  
  void 
  Viewer::add_edge(const Key	&k1,
		   const Key	&k2)
  {    
    Natural(16)		vertex_id1 = k1.to_natural16();
    Natural(16)		vertex_id2 = k2.to_natural16();
    
#ifdef VIEWER
    ubigraph_new_edge(vertex_id1, vertex_id2);
#endif
  }
  
  void 
  Viewer::remove_edge(const Key	&k1,
		      const Key	&k2)
  {    
    Natural(16)		vertex_id1 = k1.to_natural16();
    Natural(16)		vertex_id2 = k2.to_natural16();
    
    ///COMPLET IT
  }
}
