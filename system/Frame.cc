//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/system/Frame.cc
//
// created       julien quintard   [mon mar 22 02:22:43 2010]
// updated       julien quintard   [mon mar 22 21:19:29 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Frame.hh>

namespace elle
{
  namespace system
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this value defines the stack capacity in bytes i.e 256KB.
    ///
    const Natural32		Frame::Capacity = 262144;

    ///
    /// this variable defines the frame used by the application.
    ///
    /// noteworthy is that no stack is allocated since the application
    /// uses the system's stack.
    ///
    Frame			Frame::Application;

    ///
    /// this variable represents the frame used whenever an event is
    /// being processed.
    ///
    Frame*			Frame::Event = NULL;

    ///
    /// this variable points to the frame currently in use.
    ///
    Frame*			Frame::Current = NULL;

    ///
    /// this variable holds the existing frames.
    ///
    Frame::Container		Frame::Frames;

//
// ---------- XXX -------------------------------------------------------------
//

    /*
    Slot()
    {
      Frame::Switch(*Frame::Event);

      [...];

      Network::Receive()
	{
	  // Frame::Freeze() + Frame::Switch(Frame::Application)
	  Frame::Switch();

	  ...; // will come back here later.
	}
    }


    // XXX tenter avec une alloc stack + setjmp -> load stack + longjmp
    _setjmp/_longjmp (without saving signals)

    */

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the frame system.
    ///
    Status		Frame::Initialize()
    {
      enter();

      // allocate the event frame.
      Frame::Event = new Frame;

      // create the event's frame.
      if (Frame::Event->Create() == StatusError)
	escape("unable to create the event frame");

      // set the current frame as being the application.
      Frame::Current = &Frame::Application;

      // insert in the container.
      Frame::Frames.push_front(Frame::Event);

      leave();
    }

    ///
    /// this method cleans the frame system.
    ///
    Status		Frame::Clean()
    {
      enter();

      // XXX clean toutes les frames

      leave();
    }

    ///
    /// this method switches to a new frame.
    ///
    Status		Frame::Switch(Frame*			frame)
    {
      enter();

      // XXX plus utilise!!!

      leave();
    }

    ///
    /// this method dumps the frame system state.
    ///
    Status		Frame::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      Frame::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Frame] " << std::endl;

      // dump the application.
      std::cout << alignment << shift << "[Application] " << std::endl;

      if (Frame::Application.Dump(margin + 4) == StatusError)
	escape("unable to dump the application's frame");

      // dump all the frames.
      std::cout << alignment << shift << "[Frames] " << std::endl;

      for (scoutor = Frame::Frames.begin();
	   scoutor != Frame::Frames.end();
	   scoutor++)
	if ((*scoutor)->Dump(margin + 4) == StatusError)
	  escape("unable to dump the frame");

      // dump the event frame pointer.
      std::cout << alignment << shift << "[Event] "
		<< std::hex << Frame::Event << std::endl;

      // dump the current frame pointer.
      std::cout << alignment << shift << "[Current] "
		<< std::hex << Frame::Current << std::endl;

      leave();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes a new frame.
    ///
    Status		Frame::Create(const Natural32		capacity)
    {
      enter();

      // allocate the stack.
      if ((this->stack = ::malloc(capacity)) == NULL)
	escape("unable to allocate the stack");

      // set the pointers.
      this->ebp = this->stack - 4;
      this->esp = this->ebp - 4;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the frame.
    ///
    Status		Frame::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Frame] " << std::hex << this << std::endl;

      // dump the stack.
      std::cout << alignment << shift << "[Stack] "
		<< std::hex << this->stack << std::endl;

      // dump esp.
      std::cout << alignment << shift << "[ESP] "
		<< std::hex << this->esp << std::endl;

      // dump ebp
      std::cout << alignment << shift << "[EBP] "
		<< std::hex << this->ebp << std::endl;

      leave();
    }

  }
}
