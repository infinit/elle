//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/PublicKeyBlock.hh
//
// created       julien quintard   [tue feb 17 19:45:45 2009]
// updated       julien quintard   [fri apr  2 11:57:32 2010]
//

#ifndef ETOILE_KERNEL_PUBLICKEYBLOCK_HH
#define ETOILE_KERNEL_PUBLICKEYBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Address.hh>
#include <etoile/hole/Block.hh>

namespace etoile
{
  namespace kernel
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
    class PublicKeyBlock:
      public hole::Block
    {
    public:
      //
      // constructors & destructors
      //
      PublicKeyBlock();

      //
      // methods
      //
      Status		Create(const KeyPair&);

      Status		Bind();
      Status		Validate(const hole::Address&) const;

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
      PublicKey		K;
    };

  }
}

#endif
