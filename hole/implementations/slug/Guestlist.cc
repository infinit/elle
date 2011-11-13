//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien.quintard   [mon nov  7 12:39:09 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/slug/Guestlist.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Guestlist::~Guestlist()
      {
	Guestlist::Scoutor	scoutor;

	// go through the entries.
	for (scoutor = this->container.begin();
	     scoutor != this->container.end();
	     scoutor++)
	  {
	    // delete the neighbour.
	    delete scoutor->second;
	  }

	// clear the container.
	this->container.clear();
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status	Guestlist::Add(elle::Gate*		gate,
				       Host*			host)
      {
	std::pair<Guestlist::Iterator, elle::Boolean>	result;

	enter();

	// insert the host in the container.
	result = this->container.insert(Guestlist::Value(gate, host));

	// check if the insertion was successful.
	if (result.second == false)
	  escape("unable to insert the host in the container");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Guestlist::Exist(elle::Gate*		gate) const
      {
	Guestlist::Scoutor	scoutor;

	enter();

	// try to locate the gate.
	if (this->Locate(gate, scoutor) == elle::StatusTrue)
	  true();

	false();
      }

      ///
      /// XXX
      ///
      elle::Status	Guestlist::Retrieve(elle::Gate*		gate,
					    Host*&		host)
	const
      {
	Guestlist::Scoutor	scoutor;

	enter();

	// try to locate the gate.
	if (this->Locate(gate, scoutor) == elle::StatusFalse)
	  escape("unable to locate the given gate");

	// return the associated host.
	host = scoutor->second;

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Guestlist::Remove(elle::Gate*		gate)
      {
	Guestlist::Iterator	iterator;

	enter();

	// try to locate the gate.
	if (this->Locate(gate, iterator) == elle::StatusFalse)
	  escape("unable to locate the given gate");

	// erase the iterator.
	this->container.erase(iterator);

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Guestlist::Locate(elle::Gate*		gate,
					  Scoutor&		scoutor) const
      {
	Guestlist::Scoutor	s;

	enter();

	// try to locate the host.
	if ((s = this->container.find(gate)) != this->container.end())
	  {
	    // return the scoutor.
	    scoutor = s;

	    true();
	  }

	false();
      }

      ///
      /// XXX
      ///
      elle::Status	Guestlist::Locate(elle::Gate*		gate,
					  Iterator&		iterator)
      {
	Guestlist::Iterator	i;

	enter();

	// try to locate the host.
	if ((i = this->container.find(gate)) != this->container.end())
	  {
	    // return the iterator.
	    iterator = i;

	    true();
	  }

	false();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this function dumps a routing table object.
      ///
      elle::Status	Guestlist::Dump(elle::Natural32		margin) const
      {
	elle::String		alignment(margin, ' ');
	Guestlist::Scoutor	scoutor;

	enter();

	// display the name.
	std::cout << alignment << "[Guestlist]" << std::endl;

	// go through the entries.
	for (scoutor = this->container.begin();
	     scoutor != this->container.end();
	     scoutor++)
	  {
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[Guest]" << std::endl;

	    // dump the gate's address.
	    std::cout << alignment << elle::Dumpable::Shift
		      << elle::Dumpable::Shift
		      << "[Gate] " << scoutor->first << std::endl;

	    // dump the host.
	    if (scoutor->second->Dump(margin + 4) == elle::StatusError)
	      escape("unable to dump the host");
	  }

	leave();
      }

    }
  }
}
