#include <etoile/gear/Link.hh>
#include <etoile/gear/Nature.hh>

#include <etoile/nest/Nest.hh>

#include <nucleus/proton/Porcupine.hh>
#include <nucleus/neutron/Reference.hh>

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
    Link::Link():
      Object(NatureLink),

      contents_porcupine(nullptr),
      contents_nest(nullptr),
      contents_limits(nucleus::proton::limits::Porcupine{},
                      nucleus::proton::limits::Node{1048576, 0.5, 0.2},
                      nucleus::proton::limits::Node{1048576, 0.5, 0.2})
    {
    }

    ///
    /// the destructor
    ///
    Link::~Link()
    {
      delete this->contents_porcupine;
      delete this->contents_nest;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the contents along the the inherited object context.
    ///
    elle::Status        Link::Dump(const elle::Natural32        margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Link]" << std::endl;

      // dump the inherited object.
      if (Object::Dump(margin + 2) == elle::Status::Error)
        throw elle::Exception("unable to dump the inherited object");

      // dump the porcupine.
      if (this->contents_porcupine != nullptr)
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Contents] " << *this->contents_porcupine << std::endl;
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Contents] " << "none" << std::endl;
        }

      return elle::Status::Ok;
    }

  }
}
