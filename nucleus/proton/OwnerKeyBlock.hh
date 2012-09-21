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
      //
      // constructors & destructors
      //
    public:
      OwnerKeyBlock(); // XXX[to deserialize]
      OwnerKeyBlock(Network const& network,
                    neutron::Component component,
                    elle::cryptography::PublicKey const& creator_K);

      //
      // methods
      //
    public:
      /// XXX
      Address
      bind() const;
      /// XXX
      void
      validate(Address const& address) const;
      /// The subject of the block owner.
      neutron::Subject const&
      owner_subject();

      //
      // interfaces
      //
    public:
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(OwnerKeyBlock);

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
    private:
      ELLE_ATTRIBUTE_R(elle::cryptography::PublicKey, K);
      ELLE_ATTRIBUTE_R(elle::utility::Time, stamp); // XXX[creation_stamp]
      ELLE_ATTRIBUTE_R(elle::cryptography::PublicKey, owner_K);
      ELLE_ATTRIBUTE_R(elle::cryptography::Signature, owner_signature);

      neutron::Subject* _owner_subject;
    };

  }
}

# include <nucleus/proton/OwnerKeyBlock.hxx>

#endif
