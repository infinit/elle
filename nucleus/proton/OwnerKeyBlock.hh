#ifndef NUCLEUS_PROTON_OWNERKEYBLOCK_HH
# define NUCLEUS_PROTON_OWNERKEYBLOCK_HH

# include <elle/types.hh>
# include <elle/utility/Time.hh>
# include <elle/cryptography/PublicKey.hh>
# include <elle/cryptography/Signature.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/MutableBlock.hh>
# include <nucleus/neutron/Component.hh>
# include <nucleus/neutron/Subject.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

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
