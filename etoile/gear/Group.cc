#include <etoile/gear/Group.hh>
#include <etoile/gear/Nature.hh>

#include <nucleus/neutron/Role.hh>
#include <nucleus/neutron/Ensemble.hh>

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
    Group::Group():
      Context(NatureGroup),

      group(nullptr),
      ensemble(nullptr)
    {
      // initialize the rights structure.
      this->rights.role = nucleus::neutron::RoleUnknown;
    }

    ///
    /// the destructor.
    ///
    Group::~Group()
    {
      delete this->group;
      delete this->ensemble;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status
    Group::Dump(const elle::Natural32      margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Group]" << std::endl;

      // dump the parent context.
      if (Context::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the parent context");

      // dump the location.
      if (this->location.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the location");

      // dump the group.
      if (this->group != nullptr)
        {
          if (this->group->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the group");
        }
      else
        {
          // dump none.
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Group] " << elle::none << std::endl;
        }

      // dump the ensemble, if present.
      if (this->ensemble != nullptr)
        {
          // dump the ensemble block.
          if (this->ensemble->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the ensemble");
        }
      else
        {
          // dump none.
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Ensemble] " << elle::none << std::endl;
        }

      // dump the rights.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Rights]" << std::endl;

      // dump the role.
      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Role] "
                << this->rights.role << std::endl;

      return elle::Status::Ok;
    }

  }
}
