//
// ---------- header ----------------------------------------------------------
//
// project       infinite
//
// license       GPL
//
// file          infinite/hole/Key.hxx
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

//
// ---------- namespace -------------------------------------------------------
//

namespace hole
{
  ///
  /// This is the namespace of the Sdht implementation
  ///

  //
  // ---------- define ----------------------------------------------------
  //
  
  //
  // ---------- structures ------------------------------------------------------
  //


  template <class Archive>
  void Key::serialize(Archive &ar, const unsigned int &)
  {
    Integer(32)	i;
    
    for (i = 0; i < KEYLEN; i++)
    ar & k_[i];
  }        
  
}
