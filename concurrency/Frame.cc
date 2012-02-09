//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue mar 23 14:22:43 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Frame.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Frame::Frame():
      stack(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Frame::~Frame()
    {
      // release the stack.
      if (this->stack != NULL)
        ::free(this->stack);
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method allocates a new stack.
    ///
    Status              Frame::Create(const Natural32           size)
    {
      ;

      // set the size.
      this->size = size;

      // allocate the new stack.
      if ((this->stack =
           static_cast<Byte*>(::malloc(this->size))) == NULL)
        escape("unable to allocate the stack");

      return elle::StatusOk;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the stack.
    ///
    Status              Frame::Dump(const Natural32             margin) const
    {
      String            alignment(margin, ' ');

      ;

      std::cout << alignment << "[Frame] " << std::endl;

      // dump the stack address.
      std::cout << alignment << Dumpable::Shift << "[Stack] "
                << std::hex << static_cast<Void*>(this->stack) << std::endl;

      return elle::StatusOk;
    }

  }
}
