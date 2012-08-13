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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Object::Object():
      proton::ImprintBlock(ComponentObject),

      _author(new Author)
    {
      //
      // the attributes below are initialized in the constructor body
      // because they belong to a sub-structure.
      //
      this->_meta.state = proton::StateClean;
      this->_meta.owner.permissions = PermissionNone;
      this->_meta.owner.record = nullptr;
      this->_meta.genre = GenreUnknown;
      this->_meta.version = 0;

      this->_data.state = proton::StateClean;
      this->_data.size = 0;
      this->_data.version = 0;
    }

    Object::~Object()
    {
      delete this->_author;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the object given the owner public and the
    /// genre of the object to create.
    ///
    /// the method (i) starts by initializing the underlying public key block
    /// (ii) sets the meta data, and finally (iv) initializes the data
    /// part by setting the owner as the author.
    ///
    elle::Status        Object::Create(const Genre              genre,
                                       elle::cryptography::PublicKey const&   owner)
    {
      // (i)
      {
        // create the underlying owner key block.
        if (proton::ImprintBlock::Create(owner) == elle::Status::Error)
          escape("unable to create the underlying owner key block");
      }

      // (ii)
      {
        // set the meta genre.
        this->_meta.genre = genre;

        // set the initial owner permissions to all with an empty key.
        if (this->Administrate(
              this->_meta.attributes,
              PermissionRead | PermissionWrite) == elle::Status::Error)
          escape("unable to set the initial meta data");
      }

      // (iii)
      {
        Author          author;

        // set an owner author.
        if (author.Create() == elle::Status::Error)
          escape("unable to create an author");

        // set the initial data with no contents and the owner as the author.
        if (this->Update(author,
                         proton::Address::Null,
                         0,
                         proton::Address::Null,
                         this->_meta.owner.token) == elle::Status::Error)
          escape("unable to set the initial data");
      }

      return elle::Status::Ok;
    }

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
        if (this->_data.stamp.Current() == elle::Status::Error)
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
      this->state = proton::StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method updates the meta section.
    ///
    elle::Status        Object::Administrate(const Attributes&  attributes,
                                             const Permissions& permissions)
    {
      // set the last management time.
      if (this->_meta.stamp.Current() == elle::Status::Error)
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
      if (this->_meta.owner.record->Update(
            this->owner_subject(),
            this->_meta.owner.permissions,
            this->_meta.owner.token) == elle::Status::Error)
        escape("unable to create the owner access record");

      // set the the block as dirty.
      this->state = proton::StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method seals the data and meta data by signing them.
    ///
    elle::Status        Object::Seal(elle::cryptography::PrivateKey const&    k,
                                     const Access&              access)
    {
      // re-sign the data if required.
      if (this->_data.state == proton::StateDirty)
        {
          // increase the data version.
          this->_data.version += 1;

          // sign the archive with the author key.
          if (k.Sign(elle::serialize::make_tuple(
                       this->_data.contents,
                       this->_data.size,
                       this->_data.stamp,
                       this->_data.version,
                       this->_meta.owner.token,
                       this->_meta.access),
                     this->_data.signature) == elle::Status::Error)
            escape("unable to sign the data archive");

          // mark the section as consistent.
          this->_data.state = proton::StateConsistent;
        }

      // re-sign the meta data if required.
      if (this->_meta.state == proton::StateDirty)
        {
          // increase the meta version.
          this->_meta.version += 1;

          // perform the meta signature depending on the presence of a
          // reference to an access block.
          if (this->_meta.access != proton::Address::Null)
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
              elle::cryptography::Digest      fingerprint;

              // test if there is an access block.
              if (access == Access::Null)
                escape("the Seal() method must take the object's "
                       "access block");

              // compute the fingerprint of the access (subject, permissions)
              // tuples.
              if (access.Fingerprint(fingerprint) == elle::Status::Error)
                escape("unable to compute the access block fingerprint");

              // sign the meta data, making sure to include the access
              // fingerprint.
              if (k.Sign(elle::serialize::make_tuple(
                           this->_meta.owner.permissions,
                           this->_meta.genre,
                           this->_meta.stamp,
                           this->_meta.attributes,
                           this->_meta.version,
                           fingerprint),
                         this->_meta.signature) == elle::Status::Error)
                escape("unable to sign the meta archive");
            }
          else
            {
              //
              // otherwise, only the meta attributes are included in
              // the signature.
              //

              // sign the meta data.
              if (k.Sign(elle::serialize::make_tuple(
                           this->_meta.owner.permissions,
                           this->_meta.genre,
                           this->_meta.stamp,
                           this->_meta.attributes,
                           this->_meta.version),
                         this->_meta.signature) == elle::Status::Error)
                escape("unable to sign the meta archive");
            }

          // mark the section as consistent.
          this->_meta.state = proton::StateConsistent;
        }

      // set the mutable block's version.
      this->version = this->_meta.version + this->_data.version;

      // set the block as consistent.
      this->state = proton::StateConsistent;

      return elle::Status::Ok;
    }

    ///
    /// this method implements the Block's Validate() interface method.
    ///
    /// however, since the Object requires additional information in
    /// order to be validated, this method must *never* be used and therefore
    /// returns an error.
    ///
    elle::Status        Object::Validate(const proton::Address&) const
    {
      escape("this method should never have been called");
    }

    ///
    /// this method verifies that the object has been properly author
    /// i.e that every signature has been produced by legitimate users.
    ///
    /// the method (i) calls the parent class for validation (iii) verifies
    /// the meta part's signature (iv) retrieves the author's public key
    /// (v) verifies the data signature and (vi) verify that the mutable
    /// block's general version number matches the object's versions.
    ///
    elle::Status        Object::Validate(const proton::Address& address,
                                         const Access&          access)
      const
    {
      elle::cryptography::PublicKey author;

      // (i)
      {
        // call the parent class.
        if (proton::ImprintBlock::Validate(address) == elle::Status::Error)
          escape("unable to verify the underlying physical block");
      }

      // (ii)
      {
        if (this->_meta.access != proton::Address::Null)
          {
            elle::cryptography::Digest        fingerprint;

            // test if there is an access block.
            if (access == Access::Null)
              escape("the Validate() method must take the object's "
                     "access block");

            // compute the fingerprint of the access (subject, permissions)
            // tuples.
            if (access.Fingerprint(fingerprint) == elle::Status::Error)
              escape("unable to compute the access block fingerprint");

            // verify the meta part, including the access fingerprint.
            if (this->owner_K().Verify(
                  this->_meta.signature,
                  elle::serialize::make_tuple(
                    this->_meta.owner.permissions,
                    this->_meta.genre,
                    this->_meta.stamp,
                    this->_meta.attributes,
                    this->_meta.version,
                    fingerprint)) == elle::Status::Error)
              escape("unable to verify the meta's signature");
          }
        else
          {
            // verify the meta part.
            if (this->owner_K().Verify(
                  this->_meta.signature,
                  elle::serialize::make_tuple(
                    this->_meta.owner.permissions,
                    this->_meta.genre,
                    this->_meta.stamp,
                    this->_meta.attributes,
                    this->_meta.version)) == elle::Status::Error)
              escape("unable to verify the meta's signature");
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
              if (access == Access::Null)
                escape("the Validate() method must take the object's "
                       "access block");

              // retrieve the access record corresponding to the author's
              // index.
              if (access.Lookup(this->_author->lord.index,
                                record) == elle::Status::Error)
                escape("unable to retrieve the access record");

              // check the access record permissions for the given author.
              if ((record->permissions & PermissionWrite) !=
                  PermissionWrite)
                escape("the object's author does not seem to have had "
                       "the permission to modify this object");

              // check that the subject is indeed a user.
              if (record->subject.type() != Subject::TypeUser)
                escape("the author references an access record which is "
                       "not related to a user");

              // finally, set the user's public key.
              //
              // note that a copy is made to avoid any complications.
              author = record->subject.user();

              break;
            }
          case Object::RoleVassal:
            {
              // XXX to implement.

              break;
            }
          default:
            {
              escape("unexpected author's role '%u'",
                     this->_author->role);
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
                            this->_data.stamp,
                            this->_data.version,
                            this->_meta.owner.token,
                            this->_meta.access)) == elle::Status::Error)
          escape("unable to verify the data signature");
      }

      // (v)
      {
        // check the mutable block's general version.
        if (this->version != (this->_data.version + this->_meta.version))
          escape("invalid version number");
      }

      return elle::Status::Ok;
    }

    proton::Address const&
    Object::access() const
    {
      return (this->_meta.access);
    }

    Record const&
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
    Object::data_stamp() const
    {
      return (this->_data.stamp);
    }

    elle::utility::Time const&
    Object::meta_stamp() const
    {
      return (this->_meta.stamp);
    }

    proton::Version const&
    Object::data_version() const
    {
      return (this->_data.version);
    }

    proton::Version const&
    Object::meta_version() const
    {
      return (this->_meta.version);
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
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Object, _());

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
                << "[Stamp] " << std::endl;
      if (this->_meta.stamp.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the meta stamp");

      if (this->_meta.attributes.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the meta attributess");

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Access]" << std::endl;
      if (this->_meta.access.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the meta access address");

      if (this->_meta.version.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the meta version");

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
                << "[Stamp]" << std::endl;
      if (this->_data.stamp.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the data stamp");

      if (this->_data.version.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the data version");

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

  }
}
