#ifndef NUCLEUS_PROTON_IMPRINTBLOCK_HH
# define NUCLEUS_PROTON_IMPRINTBLOCK_HH

# include <elle/cryptography/PublicKey.hh>
# include <elle/serialize/construct.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/MutableBlock.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

    /// This class associates a owner to the mutable block.
    ///
    /// The functionalities offered by this construct are equivalent to
    /// the ones of OwnerKeyBlock except that it does not require the
    /// generation of a keypair which may be time-consuming though experiments
    /// show it is negligible.
    ///
    /// The idea behind this construct is very similar to public key blocks
    /// with the address being computed by applying a one-way function on
    /// the block's public key except that in this case, as for owner key
    /// blocks, the owner's public key is used as the block's public key. Thus
    /// the block's signature can be generated with the owner's private key
    /// instead of the block's private key. The result is that the user does
    /// not need to keep the private key of every created object as it is for
    /// public key blocks. Instead, the owner only keeps his personal key pair
    /// which which he created and sign imprint blocks.
    class ImprintBlock:
      public MutableBlock
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      ImprintBlock(); // XXX[to deserialize]
      ELLE_SERIALIZE_CONSTRUCT(ImprintBlock, MutableBlock)
      {
        this->_owner_subject = nullptr;
      }

      ImprintBlock(Network const& network,
                   neutron::Component const component,
                   elle::cryptography::PublicKey const& creator_K);
      ~ImprintBlock();

      /*--------.
      | Methods |
      `--------*/
    public:
      /// The subject of the block's owner.
      neutron::Subject const&
      owner_subject();

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // block
      virtual
      Address
      bind() const;
      virtual
      void
      validate(Address const& address) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(ImprintBlock);

      /*-----------.
      | Attributes |
      `-----------*/
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
