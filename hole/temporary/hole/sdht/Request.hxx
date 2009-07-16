//
// ---------- header ----------------------------------------------------------
//
// project       infinite
//
// license       GPL
//
// file          infinite/hole/sdht/Request.hxx
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../Hole.hh"
#include "../Key.hh"

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
  
  //
  // ---------- structures ------------------------------------------------------
  //


    template <class Archive>
    void Request::serialize(Archive &ar, const unsigned int &)
    {
      ar & id_;
      ar & data_key_;
      ar & type_;
      ar & ip_;
      ar & port_;
    }        

  }
}
