//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/PublicKeyBlock.hh
//
// created       julien quintard   [tue feb 17 19:45:45 2009]
// updated       julien quintard   [thu may  5 13:41:39 2011]
//

#ifndef NUCLEUS_PROTON_PUBLICKEYBLOCK_HH
#define NUCLEUS_PROTON_PUBLICKEYBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a public key block i.e a mutable block.
    ///
    /// indeed, a key pair is generated whenever such a block is created.
    /// then, since the address of such a block is computed by applying
    /// a one-way function on the generated public key, and since this
    /// public key never changes, the block's content can be modified without
    /// implied the creation of a new block as for content hash blocks.
    ///
    /// note that the version which every PKB should embed is left to
    /// the logical block to provide.
    ///
    class PublicKeyBlock:
      public Block
    {
    public:
      //
      // constructors & destructors
      //
      PublicKeyBlock();

      //
      // methods
      //
      elle::Status	Create(const elle::PublicKey&);

      elle::Status	Bind(Address&) const;
      elle::Status	Validate(const Address&) const;

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      elle::PublicKey	K;
    };

  }
}

#endif
