#include <etoile/gear/Object.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/nest/Nest.hh>

#include <nucleus/proton/Porcupine.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Author.hh>
#include <nucleus/neutron/Attributes.hh>

#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Object::Object():
      Context(NatureObject),

      access_porcupine(nullptr),
      access_nest(nullptr),
      access_limits(nucleus::proton::limits::Porcupine{},
                    nucleus::proton::limits::Node{1048576, 0.5, 0.2},
                    nucleus::proton::limits::Node{1048576, 0.5, 0.2}),

      attributes_porcupine(nullptr),
      attributes_nest(nullptr),
      attributes_limits(nucleus::proton::limits::Porcupine{},
                        nucleus::proton::limits::Node{1048576, 0.5, 0.2},
                        nucleus::proton::limits::Node{1048576, 0.5, 0.2}),

      author(nullptr)
    {
      // initialize the rights structure.
      this->rights.role = nucleus::neutron::Object::RoleUnknown;
      this->rights.permissions = nucleus::neutron::permissions::none;
      this->rights.key = nullptr;
      this->rights.record = nullptr;
    }

    ///
    /// the nature-specific constructor.
    ///
    Object::Object(const Nature                                 nature):
      Context(nature),

      access_porcupine(nullptr),
      access_nest(nullptr),
      access_limits(nucleus::proton::limits::Porcupine{},
                    nucleus::proton::limits::Node{1048576, 0.5, 0.2},
                    nucleus::proton::limits::Node{1048576, 0.5, 0.2}),

      attributes_porcupine(nullptr),
      attributes_nest(nullptr),
      attributes_limits(nucleus::proton::limits::Porcupine{},
                        nucleus::proton::limits::Node{1048576, 0.5, 0.2},
                        nucleus::proton::limits::Node{1048576, 0.5, 0.2}),

      author(nullptr)
    {
      // initialize the rights structure.
      this->rights.role = nucleus::neutron::Object::RoleUnknown;
      this->rights.permissions = nucleus::neutron::permissions::none;
      this->rights.key = nullptr;
      this->rights.record = nullptr;
    }

    ///
    /// the destructor.
    ///
    Object::~Object()
    {
      delete this->access_porcupine;
      delete this->access_nest;
      delete this->attributes_porcupine;
      delete this->attributes_nest;
      delete this->rights.key;
      delete this->rights.record;
      delete this->author;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the object.
    ///
    elle::Status        Object::Dump(const elle::Natural32      margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Object]" << std::endl;

      // dump the parent context.
      if (Context::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the parent context");

      // dump the location.
      if (this->location.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the location");

      // dump the object.
      if (this->object->Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the object");

      // dump the rights.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Rights]" << std::endl;

      // dump the role.
      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Role] "
                << this->rights.role << std::endl;

      // dump the permissions.
      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Permissions] "
                << (int)this->rights.permissions
                << std::endl;

      // dump the key.
      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Key] ";
      if (this->rights.key != nullptr)
        std::cout << *this->rights.key << std::endl;
      else
        std::cout << "null" << std::endl;

      // dump the record.
      if (this->rights.record != nullptr)
        {
          if (this->rights.record->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the record");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << elle::io::Dumpable::Shift
                    << "[Record] none" << std::endl;
        }

      // dump the author.
      if (this->author != nullptr)
        {
          if (this->author->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the author");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << elle::io::Dumpable::Shift
                    << "[Author] none" << std::endl;
        }

      return elle::Status::Ok;
    }

  }
}
