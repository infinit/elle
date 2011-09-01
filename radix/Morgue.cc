//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Morgue.cc
//
// created       julien quintard   [fri aug 26 17:10:36 2011]
// updated       julien quintard   [thu sep  1 11:17:40 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Morgue.hh>

#include <elle/concurrency/Callback.hh>

namespace elle
{
  namespace radix
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// XXX
    ///
    Morgue*				Morgue::Current = NULL;

    ///
    /// XXX
    ///
    const Natural32			Morgue::Frequency = 1000;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Morgue::Initialize()
    {
      enter();

      // allocate the morgue.
      Morgue::Current = new Morgue;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Morgue::Clean()
    {
      enter();

      // delete the morgue.
      if (Morgue::Current != NULL)
	delete Morgue::Current;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Morgue::Instance(Morgue*&		morgue)
    {
      enter();

      // verify the morgue's presence.
      if (Morgue::Current == NULL)
	false();

      morgue = Morgue::Current;

      true();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    Morgue::Morgue():
      timer(NULL)
    {
    }

    ///
    /// XXX
    ///
    Morgue::~Morgue()
    {
      // bury the pending instances.
      this->Bury();

      // if present, delete the timer.
      if (this->timer != NULL)
	delete this->timer;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Morgue::Bury()
    {
      enter();

      // as long as instances remain.
      while (this->container.empty() == false)
	{
	  Meta*		instance;

	  // retrieve the first instance.
	  instance = this->container.front();

	  // delete it.
	  delete instance;

	  // remove it from the container.
	  this->container.pop_front();
	}

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Morgue::Dump(Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Morgue::Scoutor	scoutor;

      enter();

      // display the name.
      std::cout << alignment << "[Morgue]" << std::endl;

      // go through the instances.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Meta*		instance = *scoutor;

	  // dump the instance's address.
	  std::cout << alignment << Dumpable::Shift
		    << "[Instance] " << std::hex << instance << std::endl;
	}

      leave();
    }

  }
}
