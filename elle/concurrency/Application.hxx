//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Application.hxx
//
// created       julien quintard   [tue mar 16 11:28:32 2010]
// updated       julien quintard   [sun mar 21 22:20:19 2010]
//

#ifndef ELLE_CONCURRENCY_APPLICATION_HXX
#define ELLE_CONCURRENCY_APPLICATION_HXX

namespace elle
{
  namespace concurrency
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method keeps processing events until a specific identified
    /// event is received.
    ///
    template <typename T>
    Status		Application::Wait(const Identifier&	identifier,
					  T*&			data)
    {
      Application::Value*	value;

      enter();

      // allocate a new value.
      value = new Application::Value(identifier, (Void*)NULL);

      // lock in writing.
      Application::Control.Lock(ModeWrite);
      {
	// push the new wait at the head of the waiting list.
	Application::Events.push_front(value);
      }
      Application::Control.Unlock();

      // process events.
      if (Application::Process() == StatusError)
	escape("an error occured while processing events");

      // lock in writing in order to remove the entry.
      Application::Control.Lock(ModeWrite);
      {
	// retrieve the front entry.
	value = Application::Events.front();

	// return the data.
	data = (T*)value->second;

	// delete the entry.
	delete value;

	// remove the entry.
	Application::Events.pop_front();
      }
      Application::Control.Unlock();

      leave();
    }

    ///
    /// this method tries to awaken a slot waiting for the given event.
    ///
    /// the method returns false if no such identifier was found, true
    /// if found and awaken.
    ///
    template <typename T>
    Status		Application::Awake(const Identifier&	identifier,
					   T*			data)
    {
      Application::Iterator	iterator;
      Application::Value*	value;

      enter();

      // lock in reading.
      Application::Control.Lock(ModeRead);
      {
	// check if the list of waiting events is empty.
	if (Application::Events.empty() == true)
	  {
	    Application::Control.Unlock();

	    false();
	  }

	// explore the events looking for the given identifier.
	for (iterator = Application::Events.begin();
	     iterator != Application::Events.end();
	     iterator++)
	  {
	    // test the identifier.
	    if (identifier == (*iterator)->first)
	      {
		// set the event as awaken by specifying the data pointer.
		(*iterator)->second = (Void*)data;

		Application::Control.Unlock();

		true();
	      }
	  }
      }
      Application::Control.Unlock();

      false();
    }

  }
}

#endif
