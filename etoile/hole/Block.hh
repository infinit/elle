//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/hole/Block.hh
//
// created       julien quintard   [mon feb 16 18:47:31 2009]
// updated       julien quintard   [fri sep 11 22:50:20 2009]
//

#ifndef ETOILE_HOLE_BLOCK_HH
#define ETOILE_HOLE_BLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Address.hh>

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
      // methods
      //

      ///
      /// this method computes the address of the block.
      ///
      virtual Status	Bind() const
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

      //
      // attributes
      //
      Address		address;
    };

  }
}

#endif
