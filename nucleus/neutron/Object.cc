#include <elle/cryptography/DigestSerializer.hxx>
#include <elle/serialize/TupleSerializer.hxx>
#include <elle/utility/TimeSerializer.hxx>
#include <nucleus/proton/VersionSerializer.hxx>
#include <nucleus/proton/AddressSerializer.hxx>
#include <nucleus/neutron/AttributesSerializer.hxx>
#include <nucleus/neutron/TokenSerializer.hxx>
#include <nucleus/neutron/TraitSerializer.hxx>

#include <elle/cryptography/PrivateKey.hh>

#include <nucleus/neutron/Object.hh>

#include <nucleus/neutron/Component.hh>

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
      proton::ImprintBlock(ComponentObject)
    {
      //
      // the attributes below are initialized in the constructor body
      // because they belong to a sub-structure.
      //
      this->meta.state = proton::StateClean;
      this->meta.owner.permissions = PermissionNone;
      this->meta.genre = GenreUnknown;
      this->meta.version = 0;

      this->data.state = proton::StateClean;
      this->data.size = 0;
      this->data.version = 0;
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
        this->meta.genre = genre;

        // set the initial owner permissions to all with an empty key.
        if (this->Administrate(
              this->meta.attributes,
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
                         this->meta.owner.token) == elle::Status::Error)
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
      this->author = author;

      //
      // update the elements in the data section.
      //
      {
        // set the last update time.
        if (this->data.stamp.Current() == elle::Status::Error)
          escape("unable to set the last update time");

        // set the address.
        this->data.contents = contents;

        // set the size.
        this->data.size = size;
      }

      //
      // update the elements in the meta section, though they are
      // included in the data signature.
      //
      {
        // set the address.
        this->meta.access = access;

        // set the owner token.
        this->meta.owner.token = token;
      }

      // mark the section as dirty.
      this->data.state = proton::StateDirty;

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
      if (this->meta.stamp.Current() == elle::Status::Error)
        escape("unable to set the last management time");

      // set the attributes.
      this->meta.attributes = attributes;

      // set the owner permissions.
      this->meta.owner.permissions = permissions;

      // mark the section as dirty.
      this->meta.state = proton::StateDirty;

      // re-compute the owner's access record. just like this->owner.subject,
      // this attribute is not mandatory but has been introduced in order
      // to simplify access control management.
      if (this->meta.owner.record.Update(
            this->owner.subject,
            this->meta.owner.permissions,
            this->meta.owner.token) == elle::Status::Error)
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
      if (this->data.state == proton::StateDirty)
        {
          // increase the data version.
          this->data.version += 1;

          // sign the archive with the author key.
          if (k.Sign(std::make_tuple(this->data.contents,
                                     this->data.size,
                                     this->data.stamp,
                                     this->data.version,
                                     this->meta.owner.token,
                                     this->meta.access),
                     this->data.signature) == elle::Status::Error)
            escape("unable to sign the data archive");

          // mark the section as consistent.
          this->data.state = proton::StateConsistent;
        }

      // re-sign the meta data if required.
      if (this->meta.state == proton::StateDirty)
        {
          // increase the meta version.
          this->meta.version += 1;

          // perform the meta signature depending on the presence of a
          // reference to an access block.
          if (this->meta.access != proton::Address::Null)
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
              if (k.Sign(std::make_tuple(this->meta.owner.permissions,
                                         this->meta.genre,
                                         this->meta.stamp,
                                         this->meta.attributes,
                                         this->meta.version,
                                         fingerprint),
                         this->meta.signature) == elle::Status::Error)
                escape("unable to sign the meta archive");
            }
          else
            {
              //
              // otherwise, only the meta attributes are included in
              // the signature.
              //

              // sign the meta data.
              if (k.Sign(std::make_tuple(this->meta.owner.permissions,
                                         this->meta.genre,
                                         this->meta.stamp,
                                         this->meta.attributes,
                                         this->meta.version),
                         this->meta.signature) == elle::Status::Error)
                escape("unable to sign the meta archive");
            }

          // mark the section as consistent.
          this->meta.state = proton::StateConsistent;
        }

      // set the mutable block's version.
      this->version = this->meta.version + this->data.version;

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
      elle::cryptography::PublicKey   author;

      // (i)
      {
        // call the parent class.
        if (proton::ImprintBlock::Validate(address) == elle::Status::Error)
          escape("unable to verify the underlying physical block");
      }

      // (ii)
      {
        if (this->meta.access != proton::Address::Null)
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
            if (this->owner.K.Verify(this->meta.signature,
                                     std::make_tuple(this->meta.owner.permissions,
                                                     this->meta.genre,
                                                     this->meta.stamp,
                                                     this->meta.attributes,
                                                     this->meta.version,
                                                     fingerprint)) == elle::Status::Error)
              escape("unable to verify the meta's signature");
          }
        else
          {
            // verify the meta part.
            if (this->owner.K.Verify(this->meta.signature,
                                     std::make_tuple(this->meta.owner.permissions,
                                                     this->meta.genre,
                                                     this->meta.stamp,
                                                     this->meta.attributes,
                                                     this->meta.version)) == elle::Status::Error)
              escape("unable to verify the meta's signature");
          }
      }

      // (iii)
      {
        switch (this->author.role)
          {
          case RoleOwner:
            {
              // copy the public key.
              author = this->owner.K;

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
          case RoleLord:
            {
              Record*           record;

              // check that an access block has been provided.
              if (access == Access::Null)
                escape("the Validate() method must take the object's "
                       "access block");

              // retrieve the access record corresponding to the author's
              // index.
              if (access.Lookup(this->author.lord.index,
                                record) == elle::Status::Error)
                escape("unable to retrieve the access record");

              // check the access record permissions for the given author.
              if ((record->permissions & PermissionWrite) !=
                  PermissionWrite)
                escape("the object's author does not seem to have had "
                       "the permission to modify this object");

              // check that the subject is indeed a user.
              if (record->subject.type != Subject::TypeUser)
                escape("the author references an access record which is "
                       "not related to a user");

              // finally, set the user's public key.
              //
              // note that a copy is made to avoid any complications.
              author = *record->subject.user;

              break;
            }
          case RoleVassal:
            {
              // XXX to implement.

              break;
            }
          default:
            {
              escape("unexpected author's role '%u'",
                     this->author.role);
            }
          }
      }

      // (iv)
      {
        // verify the signature.
        if (author.Verify(this->data.signature,
                          std::make_tuple(this->data.contents,
                                          this->data.size,
                                          this->data.stamp,
                                          this->data.version,
                                          this->meta.owner.token,
                                          this->meta.access)) == elle::Status::Error)
          escape("unable to verify the data signature");
      }

      // (v)
      {
        // check the mutable block's general version.
        if (this->version != (this->data.version + this->meta.version))
          escape("invalid version number");
      }

      return elle::Status::Ok;
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
      if (this->author.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the author");

      // dump the meta part.
      std::cout << alignment << elle::Dumpable::Shift << "[Meta]" << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Owner] " << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << elle::Dumpable::Shift << "[Permissions] " << std::dec
                << this->meta.owner.permissions << std::endl;

      if (this->meta.owner.token.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the meta owner's token");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Genre] " << std::dec << this->meta.genre << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Stamp] " << std::endl;
      if (this->meta.stamp.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the meta stamp");

      if (this->meta.attributes.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the meta attributess");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Access]" << std::endl;
      if (this->meta.access.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the meta access address");

      if (this->meta.version.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the meta version");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Signature]" << std::endl;
      if (this->meta.signature.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the meta signature");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[State] " << std::dec << this->meta.state << std::endl;

      // dump the data part.
      std::cout << alignment << elle::Dumpable::Shift << "[Data]" << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Contents]" << std::endl;
      if (this->data.contents.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the contents' address");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Size] " << std::dec << this->data.size << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Stamp]" << std::endl;
      if (this->data.stamp.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the data stamp");

      if (this->data.version.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the data version");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Signature]" << std::endl;
      if (this->data.signature.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the data signature");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[State] " << std::dec << this->data.state << std::endl;

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    //:w
    ////elle::Status        Object::Serialize(elle::Archive&        archive) const
    //{
    //  // call the parent class.
    //  if (proton::ImprintBlock::Serialize(archive) == elle::Status::Error)
    //    escape("unable to serialize the underlying physical block");

    //  // serialize the author part.
    //  if (archive.Serialize(this->author) == elle::Status::Error)
    //    escape("unable to serialize the author");

    //  // serialize the meta part.
    //  if (archive.Serialize(this->meta.owner.permissions,
    //                        this->meta.owner.token,
    //                        this->meta.genre,
    //                        this->meta.stamp,
    //                        this->meta.attributes,
    //                        this->meta.access,
    //                        this->meta.version,
    //                        this->meta.signature) == elle::Status::Error)
    //    escape("unable to serialize the meta part");

    //  // serialize the data part.
    //  if (archive.Serialize(this->data.contents,
    //                        this->data.size,
    //                        this->data.stamp,
    //                        this->data.version,
    //                        this->data.signature) == elle::Status::Error)
    //    escape("unable to serialize the data part");

    //  return elle::Status::Ok;
    //}

    ///
    /// this method extracts the object.
    ///
    //elle::Status        Object::Extract(elle::Archive&          archive)
    //{
    //  // call the parent class.
    //  if (proton::ImprintBlock::Extract(archive) == elle::Status::Error)
    //    escape("unable to extract the underyling physical block");

    //  // compare the component.
    //  if (this->component != ComponentObject)
    //    escape("the archive does not seem to contain an object");

    //  // extract the author part.
    //  if (archive.Extract(this->author) == elle::Status::Error)
    //    escape("unable to extract the author");

    //  // extract the meta part.
    //  if (archive.Extract(this->meta.owner.permissions,
    //                      this->meta.owner.token,
    //                      this->meta.genre,
    //                      this->meta.stamp,
    //                      this->meta.attributes,
    //                      this->meta.access,
    //                      this->meta.version,
    //                      this->meta.signature) == elle::Status::Error)
    //    escape("unable to extract the meta part");

    //  // extract the data part.
    //  if (archive.Extract(this->data.contents,
    //                      this->data.size,
    //                      this->data.stamp,
    //                      this->data.version,
    //                      this->data.signature) == elle::Status::Error)
    //    escape("unable to extract the data part");

    //  // compute the owner record.
    //  if (this->meta.owner.record.Update(
    //        this->owner.subject,
    //        this->meta.owner.permissions,
    //        this->meta.owner.token) == elle::Status::Error)
    //    escape("unable to create the owner access record");

    //  return elle::Status::Ok;
    //}

  }
}
