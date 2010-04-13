//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Hole.hh
//
// created       julien quintard   [tue apr 13 15:25:04 2010]
// updated       julien quintard   [tue apr 13 16:54:57 2010]
//

#ifndef HOLE_HOLE_HH
#define HOLE_HOLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Address.hh>
#include <hole/Data.hh>

///
/// this namespace is used by the hole storage abstraction.
///
namespace hole
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this is the base Hole class.
  ///
  class Hole
  {
  public:
    //
    // static attributes
    //
    static Hole*	Implementation;

    //
    // static methods
    //
    static Status	Initialize(const String&);
    static Status	Clean();

    static Status	Put(const Address&,
			    const Data&);
    static Status	Get(const Address&);
    static Status	Destroy(const Address&);

    //
    // interface
    //
    virtual Status	Put(const Address&,
			    const Data&) = 0;
    virtual Status	Get(const Address&) = 0;
    virtual Status	Destroy(const Address&) = 0;
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <local/Local.hh>

#endif
