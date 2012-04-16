//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [sat may  1 21:16:41 2010]
//

#ifndef LUNE_DESCRIPTOR_HH
#define LUNE_DESCRIPTOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Nucleus.hh>

#include <hole/Model.hh>

#include <lune/Authority.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents a network descriptor.
  ///
  /// note that the network name is supposed to be unique as it plays the
  /// role of identifier.
  ///
  class Descriptor:
    public elle::radix::Object,
    public elle::io::Fileable<Descriptor>
  {
  public:
    //
    // constants
    //
    static const elle::String           Extension;

    static const elle::Boolean          History;
    static const elle::Natural32        Extent;
    static const elle::Real             Contention;
    static const elle::Real             Balancing;

    //
    // methods
    //
    elle::Status        Create(const elle::String&,
                               const hole::Model&,
                               const nucleus::Address&,
                               const elle::Boolean,
                               const elle::Natural32,
                               const elle::Real&,
                               const elle::Real&);

    elle::Status        Seal(const Authority&);
    elle::Status        Validate(const Authority&) const;

    //
    // interfaces
    //

    // object
    declare(Descriptor);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // archivable
    //elle::Status        Serialize(elle::Archive&) const;
    //elle::Status        Extract(elle::Archive&);

    // fileable
    elle::Status        Load(const elle::String&);
    elle::Status        Store(const elle::String&) const;
    elle::Status        Erase(const elle::String&) const;
    elle::Status        Exist(const elle::String&) const;

    //
    // attributes
    //
    elle::String        name;
    hole::Model         model;
    nucleus::Address    root;
    elle::Boolean       history;
    elle::Natural32     extent;
    elle::Real          contention;
    elle::Real          balancing;
    elle::cryptography::Signature     signature;
  };

}

#endif
