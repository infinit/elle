//
// ---------- header ----------------------------------------------------------
//
// project       infinite
//
// license       GPL
//
// file          infinite/hole/hole.hh
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

#ifndef HOLE_HH
#define HOLE_HH

#include <elle/channel/Channel.hh>
#include <elle/misc/Report.hh>
#include <elle/misc/Network.hh>

#include "Key.hh"

using namespace elle;
using namespace elle::misc;
using namespace elle::channel;


//
// ---------- namespace -------------------------------------------------------
//

///
/// This namespace contains the hole class:
/// some distrebuted hache table implementation
///  
namespace hole
{
  
  //
  // ---------- define ----------------------------------------------------
  //
  
  //
  // ---------- structures ------------------------------------------------------
  //
  
  
  ///
  /// This class is an abstract interface for all of
  /// the DHT implementation
  ///
  ///
  class Hole
  {
  public:
    ///
    /// Seting up the local SDHT port
    ///
    Hole();
    virtual			~Hole();
    ///
    /// Create a new table and join it
    ///
    virtual Status		create() = 0;
   
    ///
    /// Join an existing DHT
    ///
    virtual Status		join(const Ip		ip,
				     const Port		port) = 0;
    
    ///
    /// Find the node corresponding to the key k
    ///
    virtual Status		lookup(const Key&) =  0;

    ///
    /// Print some usefull imformations of the current DHT
    ///
    virtual Status		print() = 0;

    //     Status		leave();
    //     Status		insert();
    //     Status		list();
    //     Status		fetch();
    
  };
  
  
  //
  // ---------- namespace -------------------------------------------------------
  //
}
    
#endif
