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
// updated       julien quintard   [fri may 28 19:05:19 2010]
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
      public elle::Object
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
      virtual elle::Status	Bind(const Universe&);
      virtual elle::Status	Validate(const Universe&,
					 const Address&) const;

      elle::Status	Load();
      elle::Status	Store() const;

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

      //
      // attributes
      //
      Family		family;
      Component		component;

      ///
      /// note that this address is never serialized.
      ///
      Address		address;
    };

  }
}

#endif
