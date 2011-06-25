//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Transcript.cc
//
// created       julien quintard   [wed jun 22 13:49:40 2011]
// updated       julien quintard   [sat jun 25 13:40:11 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Transcript.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// destructor.
    ///
    Transcript::~Transcript()
    {
      Transcript::Iterator	iterator;

      // for every action.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   iterator++)
	{
	  Action*	action = *iterator;

	  // delete the action.
	  delete action;
	}
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Transcript::Push(const nucleus::Address& address,
					 const nucleus::Block*	block)
    {
      Action*		action;

      enter(instance(action));

      // allocate an action.
      action = new Action(address, block);

      // add the action to the transcript's container.
      this->container.push_back(action);

      // waive.
      waive(action);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Transcript::Wipe(const nucleus::Address& address)
    {
      Action*		action;

      enter(instance(action));

      // allocate an action.
      action = new Action(address);

      // add the action to the transcript's container.
      this->container.push_back(action);

      // waive.
      waive(action);

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a transcript.
    ///
    elle::Status	Transcript::Dump(const elle::Natural32	margin) const
    {
      elle::String		alignment(margin, ' ');
      Transcript::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Transcript]" << std::endl;

      // for every action.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Action*	action = *scoutor;

	  // dump the action.
	  if (action->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the action");
	}

      leave();
    }

  }
}
