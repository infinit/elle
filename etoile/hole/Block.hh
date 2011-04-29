//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/hole/Block.hh
//
// created       julien quintard   [mon feb 16 18:47:31 2009]
// updated       julien quintard   [thu apr 28 18:06:53 2011]
//

#ifndef ETOILE_HOLE_BLOCK_HH
#define ETOILE_HOLE_BLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <lune/Lune.hh>

#include <etoile/hole/Address.hh>
#include <etoile/hole/Family.hh>
#include <etoile/hole/Component.hh>

namespace etoile
{
  namespace hole
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts the notion of storable block of data.
    ///
    /// note that every block is identified by an address.
    ///
    class Block:
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
      Block();
      Block(const Family&,
	    const Component&);

      //
      // methods
      //
      virtual elle::Status	Bind(Address&) const;
      virtual elle::Status	Validate(const Address&) const;

      //
      // interfaces
      //

      // object
      declare(Block);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      // fileable
      elle::Status	Load(const Address&);
      elle::Status	Store(const Address&) const;
      elle::Status	Erase(const Address&) const;
      elle::Status	Exist(const Address&) const;

      //
      // attributes
      //
      Family		family;
      Component		component;
    };

  }
}

#endif
