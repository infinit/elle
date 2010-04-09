//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          infinit/hole/sdht/Viewer.hh
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

#ifndef VIEWER_HH
#define VIEWER_HH

#include <iostream>

#include <elle/misc/Natural.hh>
#include <hole/Key.hh>

using namespace elle;
using namespace elle::miscellaneous;
using namespace hole;

namespace viewer
{
  class Viewer
  {
  public:   
    static void init();
    static void clear();
    static void add_node(const Key&);
    static void remove_node(const Key&);
    static void add_edge(const Key&, const Key&);
    static void remove_edge(const Key&, const Key&);
  };


  extern "C" 
  {
#include <stdint.h>
    
    typedef int32_t  result_t;
    typedef uint32_t vertex_id_t;
    typedef uint32_t edge_id_t;
    typedef uint32_t style_id_t;
    /* Basic API methods */
    void        ubigraph_clear();
    vertex_id_t ubigraph_new_vertex();
    edge_id_t   ubigraph_new_edge(vertex_id_t x, vertex_id_t y);
    result_t    ubigraph_remove_vertex(vertex_id_t x);
    result_t    ubigraph_remove_edge(edge_id_t e);
    /* Vertex/edge creation when user wants to use their own id's */
    result_t    ubigraph_new_vertex_w_id(vertex_id_t x);
    result_t    ubigraph_new_edge_w_id(edge_id_t e, vertex_id_t x, vertex_id_t y);
    /* Set a vertex attribute */
    result_t    ubigraph_set_vertex_attribute(vertex_id_t x,
					      const char* attribute, const char* value);
    /* Vertex styles */
    result_t    ubigraph_change_vertex_style(vertex_id_t x, style_id_t s);
    result_t    ubigraph_new_vertex_style(style_id_t parent_style);
    result_t    ubigraph_set_vertex_style_attribute(style_id_t s,
						    const char* attribute, const char* value);
    /* Set an edge attribute */
    result_t    ubigraph_set_edge_attribute(edge_id_t x,
					    const char* attribute, const char* value);
    /* Edge styles */
    result_t    ubigraph_change_edge_style(edge_id_t x, style_id_t s);
    result_t    ubigraph_new_edge_style(style_id_t parent_style);
    result_t    ubigraph_set_edge_style_attribute(style_id_t s,
						  const char* attribute, const char* value);
#define UBIGRAPH_SUCCESS   0
#define UBIGRAPH_FAIL     -1
  }
  
}

#endif

