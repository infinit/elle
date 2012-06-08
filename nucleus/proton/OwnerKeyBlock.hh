#ifndef  NUCLEUS_PROTON_OWNERKEYBLOCK_HH
# define NUCLEUS_PROTON_OWNERKEYBLOCK_HH

# include <elle/types.hh>
# include <elle/utility/Time.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/proton/MutableBlock.hh>

# include <nucleus/neutron/Component.hh>
# include <nucleus/neutron/Subject.hh>

# include <elle/idiom/Open.hh>

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
    class OwnerKeyBlock
      : public MutableBlock
      //, public elle::concept::Fileable<OwnerKeyBlock>
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
      elle::Status      Create(elle::cryptography::PublicKey const&);

      elle::Status      Bind(Address&) const;
      elle::Status      Validate(const Address&) const;

      //
      // interfaces
      //

      // object
      declare(OwnerKeyBlock);

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //ELLE_CONCEPT_FILEABLE_METHODS(OwnerKeyBlock);

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      elle::cryptography::PublicKey           K;

      elle::utility::Time                stamp;

      struct
      {
        elle::cryptography::PublicKey         K;

        elle::cryptography::Signature         signature;

        neutron::Subject        subject;
      }                         owner;
    };

  }
}

# include <nucleus/proton/OwnerKeyBlock.hxx>

#endif
