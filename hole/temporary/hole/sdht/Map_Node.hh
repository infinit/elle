//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          infinit/hole/sdht/Map_Node.hh
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

#ifndef MAP_NODE_HH
#define MAP_NODE_HH

#include <map>

#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>

#include "../Key.hh"
#include "Node.hh"
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
  /// This class represent the data container where you store
  /// your contact list
  ///
    class Map_Node
    {
    public:
      Map_Node(const Node		&me);
      Status				sendto(const Key&, const Request&);
      Status				sendall(const Request&);
      Status				look(Node&);
      Status				touch(const Key&);
      Status				print();
      std::map<Key, Node*>::iterator	begin();
      std::map<Key, Node*>::iterator	end();
      
    private:
      std::map<Key, Node*, cmp_key>	c_;
      const Node			me_;
     };
    
    
  }
}
    
#endif
