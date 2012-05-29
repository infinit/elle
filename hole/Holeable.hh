//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed may 11 14:55:32 2011]
//

#ifndef HOLE_HOLEABLE_HH
#define HOLE_HOLEABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>

namespace hole
{

  ///
  /// this class defines the interface hole implementation should implement.
  ///
  class Holeable:
    public elle::radix::Entity
  {
  public:
    //
    // constructors & destructors
    //
    Holeable(const nucleus::Network&);
    virtual ~Holeable();

    //
    // interface
    //
    virtual void                Join() = 0;
    virtual elle::Status        Leave() = 0;

    virtual elle::Status        Put(const nucleus::Address&,
                                    const nucleus::ImmutableBlock&) = 0;
    virtual elle::Status        Put(const nucleus::Address&,
                                    const nucleus::MutableBlock&) = 0;
    virtual elle::Status        Get(const nucleus::Address&,
                                    nucleus::ImmutableBlock&) = 0;
    virtual elle::Status        Get(const nucleus::Address&,
                                    const nucleus::Version&,
                                    nucleus::MutableBlock&) = 0;
    virtual elle::Status        Kill(const nucleus::Address&) = 0;

    //
    // interfaces
    //

    // dumpable
    elle::Status                Dump(const elle::Natural32 = 0) const;

    //
    // attributes
    //
    nucleus::Network            network;
  };

}

#endif
