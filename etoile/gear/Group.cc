#include <etoile/gear/Group.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/nest/Nest.hh>

#include <nucleus/neutron/Group.hh>
#include <nucleus/proton/Porcupine.hh>
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

      ensemble_porcupine(nullptr),
      ensemble_nest(nullptr),
      ensemble_limits(nucleus::proton::limits::Porcupine{},
                      nucleus::proton::limits::Node{1048576, 0.5, 0.2},
                      nucleus::proton::limits::Node{1048576, 0.5, 0.2})
    {
      // initialize the rights structure.
      this->rights.role = nucleus::neutron::Group::RoleUnknown;
    }

    Group::~Group()
    {
      delete this->ensemble_porcupine;
      delete this->ensemble_nest;
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
                    << "[Group] " << "none" << std::endl;
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
