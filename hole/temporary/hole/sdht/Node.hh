//
// ---------- header ----------------------------------------------------------
//
// project       infinite
//
// license       GPL
//
// file          infinite/hole/sdht/Node.hh
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

#ifndef NODE_HH
#define NODE_HH

#include <iostream>

#include "../Key.hh"

#include <elle/misc/Network.hh>
#include <elle/misc/Status.hh>
#include <elle/channel/Link_udpv4_sync.hh>
#include <elle/channel/Channel.hh>

//
// ---------- namespace -------------------------------------------------------
//

using namespace elle;
using namespace elle::misc;
using namespace elle::channel;

namespace hole
{
  namespace sdht
  {
    
    //
    // ---------- define ----------------------------------------------------
    //
  #define NODE_LIFE 5
    //
    // ---------- structures ------------------------------------------------------
    //
    
    ///
    /// This class represent a KEY or Index of the dht
    /// where you store datas 
    ///
    ///    
    
    class Node
    {
    public:
      Node(const Ip, const Port);
      ~Node();

      Status		print() const;
      
      Ip		ip_;
      Port		port_;
      Key		key_;
      Channel		*chan_;
      Integer(32)	life_;
      
    private:
      Node();
    };
    
    std::ostream&	operator <<  (std::ostream&, const Node&);
    
  }
}

#endif
