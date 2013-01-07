#ifndef NUCLEUS_NEUTRON_GROUP_HH
# define NUCLEUS_NEUTRON_GROUP_HH

# include <elle/types.hh>
# include <elle/serialize/Serializable.hh>

# include <cryptography/fwd.hh>
# include <cryptography/PublicKey.hh>
# include <cryptography/Signature.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

# include <nucleus/proton/ImprintBlock.hh>
# include <nucleus/proton/Address.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Token.hh>

namespace nucleus
{
  namespace neutron
  {
    /// A group represents a set of users and subgroups. Groups are used
    /// mainly to ease the process of managing accesses on files and
    /// directories by grouping entities (users for instance) together.
    ///
    /// Every group is associated with a pass which is a cryptographic key
    /// pair. While the public key of the pass is made available to everyone,
    /// the private part is distributed to every member of the group (known
    /// as fellow) through a token i.e by encrypting it with the fellow's
    /// public key so that only he can decrypt it.
    ///
    /// Whenever an object (file/directory/link) owner wishes to grant access
    /// to a group, the object secret key (with which the data have been
    /// encrypted) is encrypted with the group's public pass into a token.
    /// This token is stored in the object's Access block in the record
    /// associated with the group identity. Should a group fellow want to
    /// access the object's content, he would first decrypt his token
    /// so as to have access to the group's private pass. Then, with this
    /// private pass, he would decrypt the token contained in the Access
    /// block so as to reveal the object's secret key. Finally, the object's
    /// content could be accessed.
    ///
    /// Since the composition of the group changes over time, a fellow is
    /// removed from the group for instance, the past users must not have
    /// access to the new passes (though having access to the passes they
    /// already had access to is not a weakness of the security mechanism).
    /// For that purpose, whenever the group manager (i.e owner/creator)
    /// modifies the composition, a new pass is generated i.e the public
    /// pass is updated and the fellows' tokens are re-generated.
    ///
    /// XXX[note that a new pass must be generated only when a fellow is
    ///     removed. otherwise, there is not security threat in keeping the
    ///     same pass for new fellows]
    class Group:
      public proton::ImprintBlock,
      public elle::serialize::SerializableMixin<Group>,
      public elle::concept::UniquableMixin<Group>
    {
      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static elle::Natural32 const keypair_length;

        static Component const component;
      };

      //
      // types
      //
    public:
      /// This typedef makes it easier for other classes to define
      /// a group identity without having to know that groups are
      /// actually identified by an Address.
      typedef proton::Address Identity;

      //
      // enumerations
      //
    public:
      /// This defines the role a user can play on a group.
      enum Role
        {
          RoleUnknown = 0,
          RoleManager,
          RoleNone,

          // This is an alias for simplifying manipulating objects
          // or groups since both have similar roles.
          RoleOwner = RoleManager
        };

      //
      // constants
      //
    public:
      static const elle::Natural32 keypair_length;

      //
      // attributes
      //
    private:
      ELLE_ATTRIBUTE_R(elle::String, description);
      ELLE_ATTRIBUTE(cryptography::PublicKey*, pass_K);
      ELLE_ATTRIBUTE_RW(Size, size);
      ELLE_ATTRIBUTE_R(elle::utility::Time, modification_timestamp);
      ELLE_ATTRIBUTE_R(proton::Address, ensemble);
      ELLE_ATTRIBUTE_R(Token, manager_token);
      ELLE_ATTRIBUTE(cryptography::Signature*, signature);
      // XXX[not serialized]
      ELLE_ATTRIBUTE(Fellow*, manager_fellow); //XXX unique_ptr

      //
      // construction
      //
    public:
      Group();
      Group(proton::Network const& network,
            cryptography::PublicKey const& manager_K,
            elle::String const& description);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Group);
      ~Group();

      //
      // methods
      //
    public:
      /// Specifies the address of the new Ensemble block and thus
      /// instructs the group to upgrade itself by updating the public
      /// pass with the given one.
      void
      upgrade(proton::Address const& ensemble,
              cryptography::PublicKey const& pass_K,
              Token const& manager_token);
      /// Indicates that the group no longer references an Ensemble
      /// block. The pass is therefore no longer used.
      void
      downgrade();
      /// Seals the block by signing its content with the given manager's
      /// private key.
      void
      seal(cryptography::PrivateKey const& manager_k);
      /// Return the pass associated with the group.
      cryptography::PublicKey const&
      pass_K() const;
      /// Returns the public key of the group manager.
      cryptography::PublicKey const&
      manager_K() const;
      /// Returns the subject associated with the group manager.
      Subject const&
      manager_subject();
      /// Returns the fellow associated with the group manager.
      /// XXX[put the const back on the return type]
      Fellow&
      manager_fellow();

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serialize
      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Group);
      ELLE_SERIALIZE_FRIEND_FOR(Group);
    };
  }
}

# include <nucleus/neutron/Group.hxx>

#endif
