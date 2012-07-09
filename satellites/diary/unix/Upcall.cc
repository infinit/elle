#include <satellites/diary/unix/Upcall.hh>

namespace satellite
{
  #undef unix
  namespace unix
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the upcall by initializing the archive.
    ///
    elle::Status        Upcall::Create(const Operation          operation)
    {
      // set the operation.
      this->operation = operation;
      this->_inputs.Reset();
      this->_outputs.Reset();

      return elle::Status::Ok;
    }

    ///
    /// this method specifies the upcall's result and serializes it
    /// right away.
    ///
    elle::Status        Upcall::Result(const elle::Integer32    result)
    {
      // set the result;
      this->result = result;

      return elle::Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an diary object.
    ///
    elle::Status        Upcall::Dump(elle::Natural32            margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Upcall]" << std::endl;

      // display the operation.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Operation] " << this->operation
                << std::endl;

      // display the inputs.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Inputs]" << std::endl;

      if (this->_inputs.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the archive");

      // display the outputs.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Outputs]" << std::endl;

      if (this->_outputs.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the archive");

      // display the result.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Result] " << this->operation << std::endl;

      return elle::Status::Ok;
    }

  }
}
