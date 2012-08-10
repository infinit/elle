#include <etoile/miscellaneous/Abstract.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Role.hh>

#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace miscellaneous
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines a empty, unused hence null abstract.
    ///
    const Abstract                      Abstract::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Abstract::Abstract()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates the abstract according to the given object.
    ///
    elle::Status        Abstract::Create(const nucleus::neutron::Object& object)
    {
      // set the genre.
      this->genre = object.meta.genre;

      // set the stamps.
      this->stamps.creation = object.stamp();
      this->stamps.modification =
        object.data.stamp < object.meta.stamp ?
        object.meta.stamp :
        object.data.stamp;

      // set the size
      this->size = object.data.size;

      // set the owner.
      this->keys.owner = object.owner_K();

      // set the author depending on the mode.
      switch (object.author.role)
        {
        case nucleus::neutron::RoleOwner:
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
      this->permissions.owner = object.meta.owner.permissions;

      // set the versions.
      this->versions.meta = object.meta.version;
      this->versions.data = object.data.version;

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Abstract::operator==(const Abstract&    element)
      const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the attributes.
      if ((this->genre != element.genre) ||
          (this->stamps.creation != element.stamps.creation) ||
          (this->stamps.modification != element.stamps.modification) ||
          (this->size != element.size) ||
          (this->keys.owner != element.keys.owner) ||
          (this->keys.author != element.keys.author) ||
          (this->permissions.owner != element.permissions.owner) ||
          (this->versions.meta != element.versions.meta) ||
          (this->versions.data != element.versions.data))
        return false;

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Abstract, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the abstract object.
    ///
    elle::Status        Abstract::Dump(const elle::Natural32    margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Abstract]" << std::endl;

      // dump the genre.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Genre] "
                << this->genre << std::endl;

      //
      // dump the stamps.
      //
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Stamps]" << std::endl;

        // dump the creation time.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Creation]" << std::endl;

        if (this->stamps.creation.Dump(margin + 6) == elle::Status::Error)
          escape("unable to dump the creation time");

        // dump the modification time.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Modification]" << std::endl;

        if (this->stamps.modification.Dump(margin + 6) == elle::Status::Error)
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
      // dump the versions.
      //
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Versions]" << std::endl;

        // dump the meta version.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Meta] " << std::dec << this->versions.meta << std::endl;

        // dump the data version.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << elle::io::Dumpable::Shift
                  << "[Data] " << std::dec << this->versions.data << std::endl;
      }

      return elle::Status::Ok;
    }

  }
}
