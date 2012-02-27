//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [tue feb 17 19:45:45 2009]
//

#ifndef NUCLEUS_PROTON_PUBLICKEYBLOCK_HH
#define NUCLEUS_PROTON_PUBLICKEYBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/MutableBlock.hh>

#include <nucleus/neutron/Component.hh>

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
    /// for this construct, a key pair is generated whenever such a block
    /// is author . then, since the address of such a block is computed
    /// by applying a one-way function on the generated public key, and
    /// since this public key never changes, the block's content can be
    /// modified without implying the creation of a new block as for
    /// ContentHashBlocks.
    ///
    class PublicKeyBlock:
      public MutableBlock
    {
    public:
      //
      // constructors & destructors
      // 
      PublicKeyBlock();
      PublicKeyBlock(const neutron::Component);

      //
      // methods
      //
      elle::Status      Create(const elle::PublicKey&);

      elle::Status      Bind(Address&) const;
      elle::Status      Validate(const Address&) const;

      //
      // interfaces
      //

      // object
      declare(PublicKeyBlock);

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      elle::PublicKey   K;
    };

  }
}

#endif
