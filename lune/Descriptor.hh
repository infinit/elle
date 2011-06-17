//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Descriptor.hh
//
// created       julien quintard   [sat may  1 21:16:41 2010]
// updated       julien quintard   [fri jun 17 16:53:43 2011]
//

#ifndef LUNE_DESCRIPTOR_HH
#define LUNE_DESCRIPTOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
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
    public elle::Object,
    public virtual elle::Fileable<elle::FormatCustom>
  {
  public:
    //
    // constants
    //
    static const elle::String		Extension;

    //
    // constructors & destructors
    //
    Descriptor();

    //
    // methods
    //
    elle::Status	Create(const elle::String&,
			       const hole::Model&,
			       const nucleus::Address&,
			       const elle::Address&);

    elle::Status	Seal(const Authority&);
    elle::Status	Validate(const Authority&) const;

    //
    // interfaces
    //

    // object
    declare(Descriptor);

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    // fileable
    elle::Status	Load(const elle::String&);
    elle::Status	Store(const elle::String&) const;
    elle::Status	Erase(const elle::String&) const;
    elle::Status	Exist(const elle::String&) const;

    //
    // attributes
    //
    elle::String	name;
    hole::Model		model;
    nucleus::Address	root;
    elle::Address	boot;
    elle::Signature	signature;
  };

}

#endif
