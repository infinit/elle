//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          infinit/hole/sdht/Map_Data.hh
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

#ifndef MAP_DATA_HH
#define MAP_DATA_HH

#include <map>

#include "Key.hh"
#include "Data.hh"

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
  /// This class represent the data container where you store Data
  /// it implements data caching
  ///
  ///
    class Map_Data
    {
    public:
      Map_Data();
      Status				add(Key, Data);
      Status				print();
      std::map<Key, Data>::iterator	begin();
      std::map<Key, Data>::iterator	end();

    private:
      std::map<Key, Data, cmp_key>	cache_;
    };
    
    
  }
}
    
#endif
