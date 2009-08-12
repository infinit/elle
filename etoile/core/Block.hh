//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Block.hh
//
// created       julien quintard   [mon feb 16 18:47:31 2009]
// updated       julien quintard   [mon aug 10 21:26:57 2009]
//

#ifndef ETOILE_COMPONENTS_BLOCK_HH
#define ETOILE_COMPONENTS_BLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Address.hh>

namespace etoile
{
  namespace core
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
      public Dumpable, public Archivable
    {
    public:
      //
      // methods
      //

      ///
      /// this method computes the address of the block.
      ///
      virtual Status	Self(Address&) const = 0;

      ///
      /// this method returns StatusTrue if the block is valid, StatusFalse
      /// otherwise.
      ///
      virtual Status	Validate(const Address&) const = 0;
    };

  }
}

#endif
