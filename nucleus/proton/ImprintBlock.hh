//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sat may  7 23:26:59 2011]
//

#ifndef NUCLEUS_PROTON_IMPRINTBLOCK_HH
#define NUCLEUS_PROTON_IMPRINTBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/MutableBlock.hh>

#include <nucleus/neutron/Component.hh>
#include <nucleus/neutron/Subject.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class associates an owner to the mutable block.
    ///
    /// the functionalities offered by this construct are equivalent to
    /// the ones of OwnerKeyBlock except that it does not require the
    /// generation of a keypair which may be time consuming though experiments
    /// show it was negligible.
    ///
    /// the idea behind this construct is very similar to PublicKeyBlocks
    /// with the address being computed by applying a one-way function on
    /// the public key except that in this case, as for OwnerKeyBlocks, the
    /// owner's public key is used. thus the signature can be generated
    /// with the owner's private key i.e no additional keys need to be
    /// kept by the user.
    ///
    /// note that in addition, a stamp and salt are also included in
    /// hash in order to prevent conflicts i.e several ImprintBlocks
    /// being created by the same user at the same time.
    ///
    class ImprintBlock:
      public MutableBlock
    {
    public:
      //
      // constructors & destructors
      //
      ImprintBlock();
      ImprintBlock(const neutron::Component);

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
      declare(ImprintBlock);

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      elle::Time                stamp;
      elle::Natural64           salt;

      struct
      {
        elle::PublicKey         K;

        neutron::Subject        subject;
      }                         owner;
    };

  }
}

#endif
