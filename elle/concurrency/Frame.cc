//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Frame.cc
//
// created       julien quintard   [tue mar 23 14:22:43 2010]
// updated       julien quintard   [mon may  3 00:18:20 2010]
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
    Status		Frame::Create(const Natural32		size)
    {
      enter();

      // set the size.
      this->size = size;

      // allocate the new stack.
      if ((this->stack = (Byte*)::malloc(this->size)) == NULL)
	escape("unable to allocate the stack");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the stack.
    ///
    Status		Frame::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Frame] " << std::endl;

      // dump the stack address.
      std::cout << alignment << Dumpable::Shift << "[Stack] "
		<< std::hex << static_cast<Void*>(this->stack) << std::endl;

      leave();
    }

  }
}
