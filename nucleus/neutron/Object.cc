#include <nucleus/neutron/Object.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/neutron/Attributes.hh>
#include <nucleus/neutron/Token.hh>
#include <nucleus/neutron/Component.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Author.hh>

#include <elle/cryptography/Digest.hh>
#include <elle/serialize/TupleSerializer.hxx>
#include <elle/cryptography/PrivateKey.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- constants -------------------------------------------------------
//

    const Component Object::component = ComponentObject;

//
// ---------- constructors & destructors --------------------------------------
//

    Object::Object():
      proton::ImprintBlock(),

      _author(nullptr)
    {
      this->_meta.state = proton::StateClean;
      this->_meta.owner.permissions = permissions::none;
      this->_meta.owner.token = Token::null();
      this->_meta.owner.record = nullptr;
      this->_meta.revision = 0;

      this->_data.state = proton::StateClean;
      this->_data.size = 0;
      this->_data.revision = 0;
    }

    ///
    /// this method initializes the object.
    ///
    /// XXX
    ///
    ///
    /// this method creates the object given the owner public and the
    /// genre of the object to create.
    ///
    /// the method (i) starts by initializing the underlying public key block
    /// (ii) sets the meta data, and finally (iv) initializes the data
    /// part by setting the owner as the author.
    ///
    Object::Object(proton::Network const& network,
                   elle::cryptography::PublicKey const& owner_K,
                   Genre const genre):
      proton::ImprintBlock(network, ComponentObject, owner_K),

      _author(new Author)
    {
      //
      // the attributes below are initialized in the constructor body
      // because they belong to a sub-structure.
      //
      this->_meta.state = proton::StateClean;
      this->_meta.owner.permissions = permissions::none;
      this->_meta.owner.token = Token::null();
      this->_meta.owner.record = nullptr;
      this->_meta.revision = 0;

      this->_data.state = proton::StateClean;
      this->_data.size = 0;
      this->_data.revision = 0;

      // (i)
      {
        // set the meta genre.
        this->_meta.genre = genre;

        // set the initial owner permissions to all with an empty key.
        if (this->Administrate(
              this->_meta.attributes,
              permissions::read | permissions::write) == elle::Status::Error)
          throw Exception("unable to set the initial meta data");
      }

      // (ii)
      {
        // Set the initial data with no contents and the owner as the author.
        if (this->Update(Author(),
                         proton::Address::null(),
                         0,
                         proton::Address::null(),
                         this->_meta.owner.token) == elle::Status::Error)
          throw Exception("unable to set the initial data");
      }
    }

    Object::~Object()
    {
      delete this->_author;
      delete this->_meta.owner.record;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method updates the data section.
    ///
    elle::Status        Object::Update(const Author&            author,
                                       const proton::Address&   contents,
                                       const Size&              size,
                                       const proton::Address&   access,
                                       const Token&             token)

    {
      // set the author.
      *this->_author = author;

      //
      // update the elements in the data section.
      //
      {
        // set the last update time.
        if (this->_data.modification_timestamp.Current() == elle::Status::Error)
          escape("unable to set the last update time");

        // set the address.
        this->_data.contents = contents;

        // set the size.
        this->_data.size = size;
      }

      //
      // update the elements in the meta section, though they are
      // included in the data signature.
      //
      {
        // set the address.
        this->_meta.access = access;

        // set the owner token.
        this->_meta.owner.token = token;
      }

      // mark the section as dirty.
      this->_data.state = proton::StateDirty;

      // mark the block as dirty.
      this->state(proton::StateDirty);

      return elle::Status::Ok;
    }

    ///
    /// this method updates the meta section.
    ///
    elle::Status        Object::Administrate(const Attributes&  attributes,
                                             const Permissions& permissions)
    {
      // set the last management time.
      if (this->_meta.modification_timestamp.Current() == elle::Status::Error)
        escape("unable to set the last management time");

      // set the attributes.
      this->_meta.attributes = attributes;

      // set the owner permissions.
      this->_meta.owner.permissions = permissions;

      // mark the section as dirty.
      this->_meta.state = proton::StateDirty;

      // make sure the owner's record is computed.
      this->_owner_record();

      assert(this->_meta.owner.record != nullptr);

      // re-compute the owner's access record. just like this->owner.subject,
      // this attribute is not mandatory but has been introduced in order
      // to simplify access control management.
      delete this->_meta.owner.record;
      this->_meta.owner.record = new Record(this->owner_subject(),
                                            this->_meta.owner.permissions,
                                            this->_meta.owner.token);

      // set the the block as dirty.
      this->state(proton::StateDirty);

      return elle::Status::Ok;
    }

    ///
    /// this method seals the data and meta data by signing them.
    ///
    elle::Status        Object::Seal(elle::cryptography::PrivateKey const&    k,
                                     Access const* access)
    {
      // re-sign the data if required.
      if (this->_data.state == proton::StateDirty)
        {
          // increase the data revision.
          this->_data.revision += 1;

          // sign the archive with the author key.
          this->_data.signature =
            k.sign(elle::serialize::make_tuple(
                     this->_data.contents,
                     this->_data.size,
                     this->_data.modification_timestamp,
                     this->_data.revision,
                     this->_meta.owner.token,
                     this->_meta.access));

          // mark the section as consistent.
          this->_data.state = proton::StateConsistent;
        }

      // re-sign the meta data if required.
      if (this->_meta.state == proton::StateDirty)
        {
          // increase the meta revision.
          this->_meta.revision += 1;

          // perform the meta signature depending on the presence of a
          // reference to an access block.
          if (this->_meta.access != proton::Address::null())
            {
              //
              // if an access block is referenced, the identities and
              // permissions of the delegates must be included in the meta
              // signature.
              //
              // in practical terms, a digest of the (subject, permissions)
              // access records is computed which is then included in
              // the meta signature.
              //

              // test if there is an access block.
              if (access == nullptr)
                escape("the Seal() method must take the object's "
                       "access block");

              // compute the fingerprint of the access (subject, permissions)
              // tuples.
              elle::cryptography::Digest fingerprint(access->fingerprint());

              // sign the meta data, making sure to include the access
              // fingerprint.
              this->_meta.signature =
                k.sign(elle::serialize::make_tuple(
                         this->_meta.owner.permissions,
                         this->_meta.genre,
                         this->_meta.modification_timestamp,
                         this->_meta.attributes,
                         this->_meta.revision,
                         fingerprint));
            }
          else
            {
              //
              // otherwise, only the meta attributes are included in
              // the signature.
              //

              // sign the meta data.
              this->_meta.signature =
                k.sign(elle::serialize::make_tuple(
                         this->_meta.owner.permissions,
                         this->_meta.genre,
                         this->_meta.modification_timestamp,
                         this->_meta.attributes,
                         this->_meta.revision));
            }

          // mark the section as consistent.
          this->_meta.state = proton::StateConsistent;
        }

      // set the mutable block's revision.
      assert((this->_meta.revision + this->_data.revision) > this->revision());
      this->revision(this->_meta.revision + this->_data.revision);

      // set the block as consistent.
      this->state(proton::StateConsistent);

      return elle::Status::Ok;
    }

    proton::Address const&
    Object::access() const
    {
      return (this->_meta.access);
    }

    Record&
    Object::owner_record()
    {
      _owner_record();

      return (*this->_meta.owner.record);
    }

    Token const&
    Object::owner_token() const
    {
      return (this->_meta.owner.token);
    }

    Permissions const&
    Object::owner_permissions() const
    {
      return (this->_meta.owner.permissions);
    }

    Genre const&
    Object::genre() const
    {
      return (this->_meta.genre);
    }

    Author const&
    Object::author() const
    {
      assert(this->_author != nullptr);

      return (*this->_author);
    }

    proton::Address const&
    Object::contents() const
    {
      return (this->_data.contents);
    }

    Attributes const&
    Object::attributes() const
    {
      return (this->_meta.attributes);
    }

    Attributes&
    Object::attributes()
    {
      return (this->_meta.attributes);
    }

    Size const&
    Object::size() const
    {
      return (this->_data.size);
    }

    elle::utility::Time const&
    Object::data_modification_timestamp() const
    {
      return (this->_data.modification_timestamp);
    }

    elle::utility::Time const&
    Object::meta_modification_timestamp() const
    {
      return (this->_meta.modification_timestamp);
    }

    proton::Revision const&
    Object::data_revision() const
    {
      return (this->_data.revision);
    }

    proton::Revision const&
    Object::meta_revision() const
    {
      return (this->_meta.revision);
    }

    void
    Object::_owner_record()
    {
      // Create the record corresponding to the object owner, if necessary.
      // Note that this record will never be serialized but is used to ease
      // the process of access control since most method return a record.
      if (this->_meta.owner.record == nullptr)
        this->_meta.owner.record = new Record(this->owner_subject(),
                                              this->_meta.owner.permissions,
                                              this->_meta.owner.token);

      assert (this->_meta.owner.record != nullptr);
    }

//
// ---------- block -----------------------------------------------------------
//

    /// Implements the Block's validate() interface method.
    ///
    /// However, since the Object requires additional information in
    /// order to be validated, this method must *never* be used and therefore
    /// returns an error.
    void
    Object::validate(proton::Address const&) const
    {
      throw Exception("this method should never have been called");
    }

    void
    Object::validate(proton::Address const& address,
                     Access const* access) const
    {
      /// The method (i) calls the parent class for validation (iii) verifies
      /// the meta part's signature (iv) retrieves the author's public key
      /// (v) verifies the data signature and (vi) verify that the mutable
      /// block's general revision number matches the object's revisions.

      elle::cryptography::PublicKey author;

      // (i)
      {
        // call the parent class.
        proton::ImprintBlock::validate(address);
      }

      // (ii)
      {
        if (this->_meta.access != proton::Address::null())
          {
            // test if there is an access block.
            if (access == nullptr)
              throw Exception("the Validate() method must take the object's "
                              "access block");

            // compute the fingerprint of the access (subject, permissions)
            // tuples.
            elle::cryptography::Digest fingerprint(access->fingerprint());

            // verify the meta part, including the access fingerprint.
            if (this->owner_K().Verify(
                  this->_meta.signature,
                  elle::serialize::make_tuple(
                    this->_meta.owner.permissions,
                    this->_meta.genre,
                    this->_meta.modification_timestamp,
                    this->_meta.attributes,
                    this->_meta.revision,
                    fingerprint)) == elle::Status::Error)
              throw Exception("unable to verify the meta's signature");
          }
        else
          {
            // verify the meta part.
            if (this->owner_K().Verify(
                  this->_meta.signature,
                  elle::serialize::make_tuple(
                    this->_meta.owner.permissions,
                    this->_meta.genre,
                    this->_meta.modification_timestamp,
                    this->_meta.attributes,
                    this->_meta.revision)) == elle::Status::Error)
              throw Exception("unable to verify the meta's signature");
          }
      }

      // (iii)
      {
        switch (this->_author->role)
          {
          case Object::RoleOwner:
            {
              // copy the public key.
              author = this->owner_K();

              //
              // note that the owner's permission to write the object
              // is not checked because of the regulation mechanism.
              //
              // indeed, whenever the owner removes the write permission
              // from the object's current author, the object gets inconsistent
              // because anyone retrieving the object and verifying it would
              // conclude that the author does not have the write permission.
              //
              // to overcome this problem, the owner re-signs the data
              // in order to ensure consistency, no matter the owner's
              // permission on his own object.
              //
              // therefore, since one cannot distinguish both cases, the
              // owner's permissions are never checked through the validation
              // process. note however that by relying on the Infinit API,
              // the owner would not be able to modify his object without
              // the write permission, the software rejecting such an
              // operation.
              //

              break;
            }
          case Object::RoleLord:
            {
              Record* record;

              // check that an access block has been provided.
              if (access == nullptr)
                throw Exception("the Validate() method must take the object's "
                                "access block");

              // retrieve the access record corresponding to the author's
              // index.
              if (access->Lookup(this->_author->lord.index,
                                 record) == elle::Status::Error)
                throw Exception("unable to retrieve the access record");

              // check the access record permissions for the given author.
              if ((record->permissions() & permissions::write) !=
                  permissions::write)
                throw Exception("the object's author does not seem to have had "
                                "the permission to modify this object");

              // check that the subject is indeed a user.
              if (record->subject().type() != Subject::TypeUser)
                throw Exception("the author references an access record which "
                                "is not related to a user");

              // finally, set the user's public key.
              //
              // note that a copy is made to avoid any complications.
              author = record->subject().user();

              break;
            }
          case Object::RoleVassal:
            {
              // XXX to implement.

              break;
            }
          default:
            {
              throw Exception(
                elle::sprintf("unexpected author's role '%u'",
                              this->_author->role));
            }
          }
      }

      // (iv)
      {
        // verify the signature.
        if (author.Verify(this->_data.signature,
                          elle::serialize::make_tuple(
                            this->_data.contents,
                            this->_data.size,
                            this->_data.modification_timestamp,
                            this->_data.revision,
                            this->_meta.owner.token,
                            this->_meta.access)) == elle::Status::Error)
          throw Exception("unable to verify the data signature");
      }

      // (v)
      {
        // check the mutable block's general revision.
        if (this->revision() != (this->_data.revision + this->_meta.revision))
          throw Exception("invalid revision number");
      }
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an object.
    ///
    elle::Status        Object::Dump(elle::Natural32            margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Object]" << std::endl;

      // dump the parent class.
      if (proton::ImprintBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying owner key block");

      // dump the author part.
      if (this->_author->Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the author");

      // dump the meta part.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Meta]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Owner] " << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift << "[Permissions] " << std::dec
                << (int)this->_meta.owner.permissions << std::endl;

      if (this->_meta.owner.token.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the meta owner's token");

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Genre] " << std::dec
                << static_cast<elle::Natural32>(this->_meta.genre)
                << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Modification Timestamp] " << std::endl;
      if (this->_meta.modification_timestamp.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the meta timestamp");

      if (this->_meta.attributes.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the meta attributess");

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Access]" << std::endl;
      if (this->_meta.access.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the meta access address");

      if (this->_meta.revision.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the meta revision");

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Signature]" << std::endl;
      if (this->_meta.signature.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the meta signature");

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[State] " << std::dec << this->_meta.state << std::endl;

      // dump the data part.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Data]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Contents]" << std::endl;
      if (this->_data.contents.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the contents' address");

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Size] " << std::dec << this->_data.size << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Modification Timestamp]" << std::endl;
      if (this->_data.modification_timestamp.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the data timestamp");

      if (this->_data.revision.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the data revision");

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Signature]" << std::endl;
      if (this->_data.signature.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the data signature");

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[State] " << std::dec << this->_data.state << std::endl;

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Object::print(std::ostream& stream) const
    {
      stream << "object{"
             << this->_meta.genre
             << ", "
             << this->_data.size
             << "}";
    }

//
// ---------- operators -------------------------------------------------------
//

    std::ostream&
    operator <<(std::ostream& stream,
                Object::Role const role)
    {
      switch (role)
        {
        case Object::RoleUnknown:
          {
            stream << "unknown";
            break;
          }
        case Object::RoleOwner:
          {
            stream << "owner";
            break;
          }
        case Object::RoleLord:
          {
            stream << "lord";
            break;
          }
        case Object::RoleVassal:
          {
            stream << "vassal";
            break;
          }
        case Object::RoleNone:
          {
            stream << "none";
            break;
          }
        default:
          {
            throw Exception("unknown object roel '%s'", role);
          }
        }

      return (stream);
    }

  }
}
