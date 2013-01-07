#include <etoile/gear/Directory.hh>
#include <etoile/gear/Nature.hh>

#include <etoile/nest/Nest.hh>

#include <nucleus/proton/Porcupine.hh>
#include <nucleus/neutron/Catalog.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    Directory::Directory():
      Object(NatureDirectory),

      porcupine(nullptr),
      nest(nullptr),
      limits(nucleus::proton::limits::Porcupine{},
             nucleus::proton::limits::Node{1024, 0.5, 0.2},
             nucleus::proton::limits::Node{1024, 0.5, 0.2})
    {
    }

    ///
    /// the destructor
    ///
    Directory::~Directory()
    {
      delete this->porcupine;
      delete this->nest;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the contents along the the inherited object context.
    ///
    elle::Status        Directory::Dump(const elle::Natural32   margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Directory]" << std::endl;

      // dump the inherited object.
      if (Object::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the inherited object");

      // dump the porcupine.
      if (this->porcupine != nullptr)
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Porcupine] " << *this->porcupine << std::endl;
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Porcupine] " << "none" << std::endl;
        }

      return elle::Status::Ok;
    }

  }
}
