#include <etoile/gear/File.hh>
#include <etoile/gear/Nature.hh>

#include <etoile/nest/Nest.hh>

#include <nucleus/proton/Porcupine.hh>
#include <nucleus/neutron/Data.hh>

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
    File::File():
      Object(NatureFile),

      porcupine(nullptr),
      nest(nullptr)
    {
    }

    ///
    /// the destructor
    ///
    File::~File()
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
    elle::Status        File::Dump(const elle::Natural32        margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[File]" << std::endl;

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
                    << "[Porcupine] " << elle::none << std::endl;
        }

      return elle::Status::Ok;
    }

  }
}
