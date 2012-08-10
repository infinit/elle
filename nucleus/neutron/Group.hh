#ifndef NUCLEUS_NEUTRON_GROUP_HH
# define NUCLEUS_NEUTRON_GROUP_HH

# include <elle/types.hh>
# include <elle/concept/Serializable.hh>
# include <elle/serialize/BufferArchive.hh>
# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/PublicKey.hh>
# include <elle/cryptography/Signature.hh>

# include <nucleus/proton/ImprintBlock.hh>
# include <nucleus/proton/Address.hh>

namespace nucleus
{
  namespace neutron
  {

    /// XXX
    /// XXX whenever the group is modified: a user (not that hierarchical
    /// groups are not supported yet) is added/remove, a new keypair is
    /// generated and distributed to its members.
    class Group:
      public proton::ImprintBlock,
      public elle::concept::Serializable<Group>,
      public elle::concept::Serializable<
        Group,
        elle::serialize::BufferArchive
        >
    {
      //
      // types
      //
    public:
      typedef proton::Address Identity;

      //
      // constants
      //
    public:
      struct Constants
      {
        static const elle::Natural32 keypair_length;
      };

      //
      // construction
      //
    public:
      Group();
      Group(elle::cryptography::PublicKey const& owner_K,
            elle::String const& description);

      //
      // methods
      //
    public:
      /// XXX
      elle::cryptography::PublicKey const&
      pass() const;
      /// XXX
      proton::Address const&
      ensemble() const;
      /// XXX
      void
      upgrade(proton::Address const& ensemble,
              elle::cryptography::PublicKey const& pass);
      /// XXX
      void
      downgrade();
      /// XXX
      void
      seal(elle::cryptography::PrivateKey const& owner_k);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin = 0) const;

      // serialize
      ELLE_CONCEPT_SERIALIZABLE_METHODS(Group);
      ELLE_CONCEPT_SERIALIZABLE_METHODS(Group, elle::serialize::BufferArchive);

      ELLE_SERIALIZE_FRIEND_FOR(Group);

      //
      // attributes
      //
    private:
      elle::String _description;
      elle::cryptography::PublicKey* _pass;
      proton::Address _ensemble;
      elle::cryptography::Signature _signature;
    };

  }
}

# include <nucleus/neutron/Group.hxx>

#endif
