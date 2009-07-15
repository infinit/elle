//
// ---------- header ----------------------------------------------------------
//
// project       infinite
//
// license       GPL
//
// file          infinite/hole/sdht/Map_Request.hh
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

#ifndef MAP_REQUEST_HH
#define MAP_REQUEST_HH

#include <list>

#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>

#include "Request.hh"

//
// ---------- namespace -------------------------------------------------------
//

namespace hole
{
  namespace sdht
  {

  //
  // ---------- define ----------------------------------------------------
  //
 
  //
  // ---------- structures ------------------------------------------------------
  //
  
  
  ///
  /// This class represent the Request container where you store
  /// your pending Request list
  ///
    class Map_Request
    {
    public:
      Map_Request();

      Status					add(Request);
      Status					release(const Request::t_id);
      Status					print();
      
      std::map<Request::t_id, Request, cmp_id>	r_;
    };
  }
}
    
#endif
