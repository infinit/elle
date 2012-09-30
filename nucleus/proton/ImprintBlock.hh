#ifndef NUCLEUS_PROTON_IMPRINTBLOCK_HH
# define NUCLEUS_PROTON_IMPRINTBLOCK_HH

# include <elle/types.hh>

# include <elle/cryptography/PublicKey.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/MutableBlock.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

    /// This class associates an owner to the mutable block.
    ///
    /// The functionalities offered by this construct are equivalent to
    /// the ones of OwnerKeyBlock except that it does not require the
    /// generation of a keypair which may be time consuming though experiments
    /// show it is negligible.
    ///
    /// The idea behind this construct is very similar to PublicKeyBlocks
    /// with the address being computed by applying a one-way function on
    /// the public key except that in this case, as for OwnerKeyBlocks, the
    /// owner's public key is used as the block's public key. Thus the
    /// signature can be generated with the owner's private key i.e no
    /// additional keys need to be kept by the user but its own keypair.
    ///
    /// Note that in addition, a stamp and salt are also included and
    /// hashed in order to prevent conflicts i.e several ImprintBlocks
    /// being created by the same user at the same time.
    class ImprintBlock:
      public MutableBlock
    {
      //
      // construction
      //
    public:
      ImprintBlock(); // XXX[to deserialize]
      ImprintBlock(Network const& network,
                   neutron::Component const component,
                   elle::cryptography::PublicKey const& creator_K);
      ~ImprintBlock();

      //
      // methods
      //
    public:
      /// The subject of the block's owner.
      neutron::Subject const&
      owner_subject();

      //
      // interfaces
      //
    public:
      // block
      virtual
      Address
      bind() const;
      virtual
      void
      validate(Address const& address) const;
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(ImprintBlock);

      //
      // attributes
      //
    private:
      /// The block's owner public key: the complementary private key
      /// must be used to sign the block's content. Note that the signature
      /// is not embeded in this class. Instead the higher-level classes
      /// include the signature, or several ones should it be necessary.
      ELLE_ATTRIBUTE_R(elle::cryptography::PublicKey, owner_K);
      /// This subject is generated on-demand in order to ease the management
      /// of the block owner's identity. The attribute is therefore never
      /// serialized.
      ELLE_ATTRIBUTE(neutron::Subject*, owner_subject);
    };

  }
}

# include <nucleus/proton/ImprintBlock.hxx>

#endif
