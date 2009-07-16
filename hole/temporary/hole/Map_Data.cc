//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinit/hole/Map_Data.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//


#include "Map_Data.hh"

#include <map>
#include <elle/misc/Status.hh>

namespace hole
{
  namespace sdht
  { 
    Map_Data::Map_Data()
    {
    }

    Status
    Map_Data::add(const Key	k,
	       const Data	d)
    {
      cache_[k] = d;
      return StatusOk;
    }

    std::map<Key, Data>::iterator	
    Map_Data::begin()
    {
      return cache_.begin();
    }
      
    std::map<Key, Data>::iterator	
    Map_Data::end()
    {
      return cache_.end();
    }

    
    Status
    Map_Data::print()
    {
      std::map<Key, Data>::iterator it;
      
      std::cout << "Map_Data list:" << std::endl;  
      if (cache_.size() == 0)
	{
	  std::cout << "   Empty" << std::endl;
	  return StatusOk;
	}
      
      for (it = cache_.begin(); it != cache_.end(); it++)
	std::cout << "   Key: " << it->first << " data: " << it->second << std::endl;
      return StatusOk;
    }

  }
}
