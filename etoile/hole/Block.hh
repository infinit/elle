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
// updated       julien quintard   [thu apr 22 17:12:34 2010]
//

#ifndef ETOILE_HOLE_BLOCK_HH
#define ETOILE_HOLE_BLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constructors & destructors
      //
      Block();
      Block(const Family&,
	    const Component&);

      //
      // methods
      //

      ///
      /// this method computes the address of the block.
      ///
      virtual Status	Bind()
      {
	fail("this method should never have been called");
      }

      ///
      /// this method returns StatusTrue if the block is valid, StatusFalse
      /// otherwise.
      ///
      virtual Status	Validate(const Address&) const
      {
	fail("this method should never have been called");
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

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
