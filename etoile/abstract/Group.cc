#include <etoile/abstract/Group.hh>
#include <nucleus/neutron/Group.hh>

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
    Group::Group()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates the abstract according to the given group.
    ///
    elle::Status        Group::Create(const nucleus::neutron::Group& group)
    {
      this->description = group.description();

      this->timestamps.creation = group.creation_timestamp();
      this->timestamps.modification = group.modification_timestamp();

      this->size = group.size();

      this->manager = group.manager_K();

      this->revision = group.revision();

      this->pass_K = group.pass_K();

      return elle::Status::Ok;
    }

//
// ---------- group ----------------------------------------------------------
//

    ///
    /// this operator compares two groups.
    ///
    elle::Boolean       Group::operator==(const Group&    element)
      const
    {
      // check the address as this may actually be the same group.
      if (this == &element)
        return true;

      // compare the attributes.
      if ((this->description != element.description) ||
          (this->timestamps.creation != element.timestamps.creation) ||
          (this->timestamps.modification != element.timestamps.modification) ||
          (this->size != element.size) ||
          (this->manager != element.manager) ||
          (this->revision != element.revision) ||
          (this->pass_K != element.pass_K))
        return false;

      return true;
    }

    ///
    /// this macro-function call generates the group.
    ///
    embed(Group, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the abstract group.
    ///
    elle::Status        Group::Dump(const elle::Natural32    margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Group]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[Description] "
                << this->description << std::endl;

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

      // dump the manager public key.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Manager]" << std::endl;

      if (this->manager.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the manager public key");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Revision] " << std::dec << this->revision << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Pass K]" << std::endl;

      if (this->pass_K.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the pass");

      return elle::Status::Ok;
    }

  }
}
