#include <etoile/gear/Context.hh>

#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Context::Context(const Nature                               nature):
      nature(nature),
      state(Context::StateUnknown),
      operation(OperationUnknown)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an context object.
    ///
    elle::Status        Context::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      // display the name.
      std::cout << alignment << "[Context] " << std::endl;

      // display the nature.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Nature] "
                << this->nature << std::endl;

      // display the state.
      std::cout << alignment << elle::io::Dumpable::Shift << "[State] "
                << this->state << std::endl;

      // display the operation.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Operation] "
                << this->operation << std::endl;

      // dump the transcript.
      if (this->transcript.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the transcript");

      return elle::Status::Ok;
    }

  }
}
