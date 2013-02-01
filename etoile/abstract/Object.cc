#include <etoile/abstract/Object.hh>
#include <nucleus/neutron/Object.hh>

namespace etoile
{
  namespace abstract
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Object::Object()
    {
      this->keys.owner = nullptr;
      this->keys.author = nullptr;
    }

    Object::Object(Object const& other)
    {
      this->genre = other.genre;

      this->timestamps.creation = other.timestamps.creation;
      this->timestamps.modification = other.timestamps.modification;

      this->size = other.size;

      this->keys.owner = new cryptography::PublicKey{*other.keys.owner};
      this->keys.author = new cryptography::PublicKey{*other.keys.author};

      this->permissions.owner = other.permissions.owner;

      this->revisions.data = other.revisions.data;
      this->revisions.meta = other.revisions.meta;
    }

    Object::~Object()
    {
      delete this->keys.owner;
      delete this->keys.author;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates the abstract according to the given object.
    ///
    elle::Status        Object::Create(const nucleus::neutron::Object& object)
    {
      // set the genre.
      this->genre = object.genre();

      // set the timestamps.
      this->timestamps.creation = object.creation_timestamp();
      this->timestamps.modification =
        object.data_modification_timestamp() < object.meta_modification_timestamp() ?
        object.meta_modification_timestamp() :
        object.data_modification_timestamp();

      // set the size
      this->size = object.size();

      // set the owner.
      ELLE_ASSERT(this->keys.owner == nullptr);
      this->keys.owner = new cryptography::PublicKey{object.owner_K()};

      // set the author depending on the mode.
      ELLE_ASSERT(this->keys.author == nullptr);
      switch (object.author().role)
        {
        case nucleus::neutron::Object::RoleOwner:
          {
            this->keys.author = new cryptography::PublicKey{object.owner_K()};

            break;
          }
        case nucleus::neutron::Object::RoleLord:
          {
            // XXX[this is wrong: we should look into the ACL]
            this->keys.author = new cryptography::PublicKey{object.owner_K()};

            break;
          }
        default:
          {
            // XXX to implement.
          }
        }
      ELLE_ASSERT(this->keys.author != nullptr);

      // set the permissions.
      this->permissions.owner = object.owner_permissions();

      // set the revisions.
      this->revisions.meta = object.meta_revision();
      this->revisions.data = object.data_revision();

      return elle::Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the abstract object.
    ///
    elle::Status        Object::Dump(const elle::Natural32    margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Object]" << std::endl;

      // dump the genre.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Genre] "
                << this->genre << std::endl;

      //
      // dump the timestamps.
      //
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Timestamps]" << std::endl;

        // dump the creation time.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Creation]" << std::endl;

        if (this->timestamps.creation.Dump(margin + 6) == elle::Status::Error)
          throw elle::Exception("unable to dump the creation time");

        // dump the modification time.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Modification]" << std::endl;

        if (this->timestamps.modification.Dump(margin + 6) == elle::Status::Error)
          throw elle::Exception("unable to dump the modification time");
      }

      // dump the size.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Size] "
                << std::dec << this->size << std::endl;

      //
      // dump the public keys.
      //
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Keys]" << std::endl;

        // dump the owner public key.
        ELLE_ASSERT(this->keys.owner != nullptr);
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Owner] " << this->keys.owner << std::endl;

        // dump the author public key.
        ELLE_ASSERT(this->keys.author != nullptr);
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Author] " << this->keys.author << std::endl;
      }

      //
      // dump the permissions.
      //
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Permissions]" << std::endl;

        // dump the owner permissions.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Owner] " << (int)this->permissions.owner << std::endl;
      }

      //
      // dump the revisions.
      //
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Revisions]" << std::endl;

        // dump the meta revision.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Meta] " << std::dec << this->revisions.meta << std::endl;

        // dump the data revision.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Data] " << std::dec << this->revisions.data << std::endl;
      }

      return elle::Status::Ok;
    }

  }
}
