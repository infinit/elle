#include <etoile/abstract/Object.hh>
#include <nucleus/neutron/Object.hh>

#include <elle/idiom/Open.hh>

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
      this->keys.owner = object.owner_K();

      // set the author depending on the mode.
      switch (object.author().role)
        {
        case nucleus::neutron::Object::RoleOwner:
          {
            this->keys.author = object.owner_K();

            break;
          }
        default:
          {
            // XXX to implement.
          }
        }

      // set the permissions.
      this->permissions.owner = object.owner_permissions();

      // set the revisions.
      this->revisions.meta = object.meta_revision();
      this->revisions.data = object.data_revision();

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Object::operator==(const Object&    element)
      const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the attributes.
      if ((this->genre != element.genre) ||
          (this->timestamps.creation != element.timestamps.creation) ||
          (this->timestamps.modification != element.timestamps.modification) ||
          (this->size != element.size) ||
          (this->keys.owner != element.keys.owner) ||
          (this->keys.author != element.keys.author) ||
          (this->permissions.owner != element.permissions.owner) ||
          (this->revisions.meta != element.revisions.meta) ||
          (this->revisions.data != element.revisions.data))
        return false;

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Object, _());

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
          escape("unable to dump the creation time");

        // dump the modification time.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Modification]" << std::endl;

        if (this->timestamps.modification.Dump(margin + 6) == elle::Status::Error)
          escape("unable to dump the modification time");
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
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Owner]" << std::endl;

        if (this->keys.owner.Dump(margin + 6) == elle::Status::Error)
          escape("unable to dump the owner public key");

        // dump the author public key.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Author]" << std::endl;

        if (this->keys.author.Dump(margin + 6) == elle::Status::Error)
          escape("unable to dump the author public key");
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
