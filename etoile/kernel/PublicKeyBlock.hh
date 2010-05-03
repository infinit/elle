//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/PublicKeyBlock.hh
//
// created       julien quintard   [tue feb 17 19:45:45 2009]
// updated       julien quintard   [mon may  3 16:44:48 2010]
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
      PublicKeyBlock(const hole::Component&);

      //
      // methods
      //
      elle::Status	Create(const elle::PublicKey&);

      elle::Status	Bind();
      elle::Status	Validate(const hole::Address&) const;

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
      elle::PublicKey		K;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Family.hh>
#include <etoile/hole/Component.hh>

#endif
