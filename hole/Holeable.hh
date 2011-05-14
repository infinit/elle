//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Holeable.hh
//
// created       julien quintard   [wed may 11 14:55:32 2011]
// updated       julien quintard   [fri may 13 11:39:04 2011]
//

#ifndef HOLE_HOLEABLE_HH
#define HOLE_HOLEABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace hole
{

  ///
  /// XXX
  ///
  class Holeable
  {
  public:
    //
    // constructors & destructors
    //
    Holeable(const nucleus::Network&);
    virtual ~Holeable();

    //
    // methods
    //
    elle::Status		Root(const nucleus::Address&);

    //
    // interface
    //
    virtual elle::Status	Join() = 0;
    virtual elle::Status	Leave() = 0;

    virtual elle::Status	Put(const nucleus::Address&,
				    const nucleus::Block&) = 0;
    virtual elle::Status	Get(const nucleus::Address&,
				    nucleus::Block&) = 0;
    virtual elle::Status	Gather(const nucleus::Address&,
				       nucleus::Block&) = 0;
    virtual elle::Status	Erase(const nucleus::Address&) = 0;

    //
    // attributes
    //
    nucleus::Network	network;
    nucleus::Address	root;
  };

}

#endif
