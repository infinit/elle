//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri may  6 14:45:42 2011]
//

#ifndef NUCLEUS_PROTON_OWNERKEYBLOCK_HH
#define NUCLEUS_PROTON_OWNERKEYBLOCK_HH

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
    /// this class represents a mutable block which is statically linked
    /// to a specific user i.e the block owner.
    ///
    /// this construct is interesting because, given multiple such blocks,
    /// the owner only has to remember her personal key pair in order to
    /// update them rather than keeping the private key associated with
    /// with every mutable block as for PKBs.
    ///
    class OwnerKeyBlock:
      public MutableBlock
    {
    public:
      //
      // constructors & destructors
      //
      OwnerKeyBlock();
      OwnerKeyBlock(const neutron::Component);

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
      declare(OwnerKeyBlock);

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      elle::PublicKey           K;

      elle::Time                stamp;

      struct
      {
        elle::PublicKey         K;

        elle::Signature         signature;

        neutron::Subject        subject;
      }                         owner;
    };

  }
}

#endif
