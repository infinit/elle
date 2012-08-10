#ifndef NUCLEUS_PROTON_IMPRINTBLOCK_HH
# define NUCLEUS_PROTON_IMPRINTBLOCK_HH

# include <elle/types.hh>

# include <elle/utility/Time.hh>
# include <elle/cryptography/PublicKey.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/MutableBlock.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

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
      public MutableBlock,
      public elle::concept::Serializable<ImprintBlock>
    {
      //
      // constructors & destructors
      //
    public:
      ImprintBlock();
      ImprintBlock(neutron::Component const);
      ImprintBlock(neutron::Component const,
                   elle::cryptography::PublicKey const&);
      ~ImprintBlock();

      //
      // methods
      //
    public:
      /// XXX
      elle::Status
      Create(elle::cryptography::PublicKey const&);
      /// XXX
      elle::Status
      Bind(Address&) const;
      /// XXX
      elle::Status
      Validate(Address const&) const;
      /// XXX
      elle::utility::Time const&
      stamp() const;
      /// XXX
      elle::cryptography::PublicKey const&
      owner_K() const;
      /// XXX
      neutron::Subject const&
      owner_subject();

      //
      // interfaces
      //
    public:
      // object
#include <elle/idiom/Open.hh>
      declare(ImprintBlock);
#include <elle/idiom/Close.hh>

      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      // serializable
      ELLE_CONCEPT_SERIALIZABLE_METHODS(ImprintBlock);

      ELLE_SERIALIZE_FRIEND_FOR(ImprintBlock);

      //
      // attributes
      //
    private:
      elle::utility::Time _stamp;
      elle::Natural64 _salt;

      struct
      {
        elle::cryptography::PublicKey K;

        neutron::Subject* subject;
      } _owner;
    };

  }
}

# include <nucleus/proton/ImprintBlock.hxx>

#endif
