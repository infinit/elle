//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          infinit/hole/sdht/Sdht.hh
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

#ifndef SDHT_HH
#define SDHT_HH

#include <boost/thread/thread.hpp>
#include <list>

#include "../Hole.hh"
#include "../Key.hh"
#include "../Map_Data.hh"

#include "Request.hh"
#include "Map_Node.hh"
#include "Map_Request.hh"

//
// ---------- namespace -------------------------------------------------------
//

namespace hole
{
  ///
  /// This is the namespace of the Sdht implementation
  ///
  namespace sdht
  {

  //
  // ---------- define ----------------------------------------------------
  //
  #define LOOK_TIME 1
  //
  // ---------- structures ------------------------------------------------------
  //
  
  ///
  /// This class is a Chord based implementation of DHT
  /// Small DHT: Small because it is a draft for futures
  /// roxoring implementation
    class Sdht:		public Hole
    {
    public:
      Sdht(const Ip&, const Port&);
      ~Sdht();
      Status			create();
      Status			join(const Ip		ip,
				     const Port		port);
      Status			lookup(const Key&);
      Status			print();
      
    private:
      Status			look();
      Status			looking();
      
      Status			listen();
      Status			listening();
      
      Status			ping();
      Status			stabilize();
      
      Status			lookup_request(Request&);
      Status			lookup_find(Request&);
      Status			ping_request();
      

      Status			ping_response(Request&);
      Status			pong_response(Request&);
      Status			lookup_response(Request&);
      Status			default_response(Request&);


      Node			me_;
      
      Map_Data			m_data_;
      Map_Node			m_node_;
      Map_Request		m_request_;

      boost::thread		*listener_;
      boost::thread		*looker_;
    };
    
    
  }
}
    
#endif
