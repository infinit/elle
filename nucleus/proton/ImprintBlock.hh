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
      ImprintBlock();
      ImprintBlock(neutron::Component const);
      ImprintBlock(neutron::Component const,
                   elle::cryptography::PublicKey const&);
      ~ImprintBlock();

      //
      // methods
      //
    public:
      /// Creates an imprint based on the given owner's public key.
      elle::Status
      Create(elle::cryptography::PublicKey const& owner_K);
      /// Computes the address of the block.
      elle::Status
      Bind(Address&) const;
      /// Returns true if the block is consistent with the given address.
      elle::Status
      Validate(Address const& address) const;
      /// The timestamp associated with the creating of the block.
      elle::utility::Time const&
      creation_stamp() const;
      /// The public key of the block's owner.
      elle::cryptography::PublicKey const&
      owner_K() const;
      /// The subject of the block's owner.
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
      // XXX[should not be the case since in-the-middle class] ELLE_SERIALIZE_SERIALIZABLE_METHODS(ImprintBlock);

      ELLE_SERIALIZE_FRIEND_FOR(ImprintBlock);

      //
      // attributes
      //
    private:
      /// The block creation timestamp.
      elle::utility::Time _creation_stamp;
      /// A random salt so as to differentiate two ImprintBlocks being
      /// created by the same user at the exact same time.
      elle::Natural64 _salt;

      struct
      {
        /// The block's owner public key: the complementary private key
        /// must be used to sign the block's content. Note that the signature
        /// is not embeded in this class. Instead the higher-level classes
        /// include the signature, or several ones should it be necessary.
        elle::cryptography::PublicKey K;

        /// This subject is generated on-demand in order to ease the management
        /// of the block owner's identity. The attribute is therefore never
        /// serialized.
        neutron::Subject* subject;
      } _owner;
    };

  }
}

# include <nucleus/proton/ImprintBlock.hxx>

#endif
