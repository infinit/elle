#ifndef NUCLEUS_NEUTRON_OBJECT_HH
# define NUCLEUS_NEUTRON_OBJECT_HH

# include <elle/types.hh>
# include <elle/utility/Time.hh>
# include <elle/serialize/Serializable.hh>
# include <elle/serialize/construct.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/proton/ImprintBlock.hh>
# include <nucleus/proton/Revision.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Genre.hh>
# include <nucleus/neutron/Size.hh>
# include <nucleus/neutron/Permissions.hh>
# include <nucleus/neutron/Token.hh>
# include <nucleus/neutron/Attributes.hh>
# include <nucleus/neutron/Record.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class is the most important of the whole Infinit project
    /// as it describes file system objects being files, directories and
    /// references.
    ///
    /// an object can rely on several physical constructors such as an
    /// ImprintBlock, an OwnerKeyBlock or else.
    ///
    /// basically, an object contains the author i.e the user having performed
    /// the latest modification along with the address of the Access block
    /// which contains the set of authorised users and groups. note however
    /// than most objects do not reference an access block in which cases
    /// the object is considered private i.e only its owner has access.
    ///
    /// in addition, several meta information are contained such as the
    /// owner permissions, some timestamps, the attributes etc.
    ///
    /// finally, the data section contains the address of the object's
    /// contents though the nature of the contents depends upon the
    /// object's nature: file, directory or reference.
    ///
    /// noteworthy is tha meta.owner.record is generated in order to
    /// make it as easy to manipulate the owner entry as for other access
    /// records. thus, this attribute is never serialized.
    ///
    class Object:
      public proton::ImprintBlock,
      public elle::serialize::SerializableMixin<Object>,
      public elle::concept::MakeUniquable<Object>
    {
      //
      // constants
      //
    public:
      static const Component component;

      //
      // enumerations
      //
    public:
      /// This defines the roles that a user can play on an object.
      enum Role
        {
          RoleUnknown,
          RoleOwner,
          RoleLord,
          RoleVassal,
          RoleNone
        };

      //
      // constructors & destructors
      //
    public:
      Object(proton::Network const& network,
             elle::cryptography::PublicKey const& owner_K,
             Genre const genre);

      Object(); // XXX[use deserialize constructor]

      ELLE_SERIALIZE_CONSTRUCT(Object, ImprintBlock)
      { this->_author = nullptr; }

      ~Object();

      //
      // methods
      //
    public:
      elle::Status      Update(const Author& author,
                               const proton::Address& contents,
                               const Size& size,
                               const proton::Address& access,
                               const Token& owner_token);
      elle::Status      Administrate(const Attributes&,
                                     const Permissions&);

      elle::Status      Seal(elle::cryptography::PrivateKey const&,
                             const Access&);

      /// Returns the address of the referenced Access block.
      proton::Address const&
      access() const;
      /// Returns the record associated with the object owner.
      /// XXX[the const on the return type needs to be added]
      Record&
      owner_record();
      /// Returns the owner's access token.
      Token const&
      owner_token() const;
      /// Returns the owner's access permissions.
      Permissions const&
      owner_permissions() const;
      /// Returns the object's genre: file, directory or link.
      Genre const&
      genre() const;
      /// Returns the object's author i.e last writer.
      Author const&
      author() const;
      /// Returns the address of the contents block.
      proton::Address const&
      contents() const;
      /// Returns the attributes associated with the object.
      Attributes const&
      attributes() const;
      /// Returns the attributes associated with the object.
      Attributes&
      attributes();
      /// Returns the size of the object's content.
      Size const&
      size() const;
      /// Returns the timestamp associated with the data section.
      elle::utility::Time const&
      data_modification_timestamp() const;
      /// Returns the timestamp associated with the meta section.
      elle::utility::Time const&
      meta_modification_timestamp() const;
      /// Returns the revision associated with the data section.
      proton::Revision const&
      data_revision() const;
      /// Returns the revision associated with the meta section.
      proton::Revision const&
      meta_revision() const;

    private:
      /// Computes the owner's record, if necessary.
      void
      _owner_record();

      //
      // interfaces
      //
    public:
      // block
      virtual
      void
      validate(proton::Address const& address) const;
      void
      validate(proton::Address const& address,
               Access const& access) const;
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Object);
      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Object);

      //
      // attributes
      //
    private:
      Author* _author;

      struct
      {
        // XXX to implement: proton::Base               base;

        struct
        {
          Permissions permissions;
          Token token;
          Record* record;
        } owner;

        Genre genre;
        elle::utility::Time modification_timestamp;

        Attributes attributes;

        proton::Address access;

        proton::Revision revision;
        elle::cryptography::Signature signature;

        proton::State state;
      } _meta;

      struct
      {
        // XXX to implement: proton::Base               base;

        proton::Address contents;

        Size size;
        elle::utility::Time modification_timestamp;

        proton::Revision revision;
        elle::cryptography::Signature signature;

        proton::State state;
      } _data;
    };

    std::ostream&
    operator <<(std::ostream& stream,
                Object::Role const role);

  }
}

# include <nucleus/neutron/Object.hxx>

#endif
