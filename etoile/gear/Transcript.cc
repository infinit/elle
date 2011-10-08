//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed jun 22 13:49:40 2011]
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
      // flush the container.
      this->Flush();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method records the given address/block as needed to be
    /// pushed onto the storage layer.
    ///
    elle::Status	Transcript::Push(const nucleus::Address& address,
					 const nucleus::Block*	block)
    {
      Action*		action;

      enterx(instance(action));

      // allocate an action.
      action = new Action(address, block);

      // add the action to the transcript's container.
      this->container.push_back(action);

      // waive.
      waive(action);

      leave();
    }

    ///
    /// this method records the given address as needed to be removed
    /// from the storage layer.
    ///
    elle::Status	Transcript::Wipe(const nucleus::Address& address)
    {
      Action*		action;

      enterx(instance(action));

      // allocate an action.
      action = new Action(address);

      // add the action to the transcript's container.
      this->container.push_back(action);

      // waive.
      waive(action);

      leave();
    }

    ///
    /// this method clears the transcript from some previously registered
    /// actions i.e depending on the given type of action.
    ///
    elle::Status	Transcript::Clear(const Action::Type	type)
    {
      Transcript::Iterator	iterator;

      enter();

      // for every action.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   )
	{
	  Action*	action = *iterator;

	  // ignore actions mismatching the given type.
	  if (action->type != type)
	    {
	      // increase the iterator.
	      iterator++;

	      continue;
	    }

	  // delete the action.
	  delete action;

	  // erase the entry.
	  this->container.erase(iterator);

	  // reset the iterator.
	  iterator = this->container.begin();
	}

      leave();
    }

    ///
    /// this method flushes the transcript from the previously registered
    /// actions.
    ///
    elle::Status	Transcript::Flush()
    {
      Transcript::Iterator	iterator;

      enter();

      // for every action.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   iterator++)
	{
	  Action*	action = *iterator;

	  // delete the action.
	  delete action;
	}

      // clear the container.
      this->container.clear();

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
