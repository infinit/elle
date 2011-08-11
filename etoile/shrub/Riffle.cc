//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/shrub/Riffle.cc
//
// created       julien quintard   [mon aug  8 13:03:14 2011]
// updated       julien quintard   [thu aug 11 10:30:14 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/shrub/Riffle.hh>
#include <etoile/shrub/Shrub.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace shrub
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Riffle::Riffle():
      parent(NULL)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Riffle::Create(const path::Slab&	slab,
				       const nucleus::Location&	location,
				       Riffle*			parent)
    {
      enter();

      // set the attributes.
      this->slab = slab;
      this->location = location;
      this->parent = parent;

      // retrieve the current time.
      if (this->timestamp.Current() == elle::StatusError)
	escape("unable to retrieve the current time");

      leave();
    }

    ///
    /// this method tries to resolve a slab within the riffle's scope.
    ///
    elle::Status	Riffle::Resolve(const path::Slab&	slab,
					Riffle*&		riffle)
    {
      Riffle::Scoutor	scoutor;

      enter();

      // initialize the pointer.
      riffle = NULL;

      // look up the element in the current riffle ... stop if not present.
      if ((scoutor = this->children.find(slab)) == this->children.end())
	leave();

      // return the resolved riffle.
      riffle = scoutor->second;

      leave();
    }

    ///
    /// this method updates the riffle with the given slab/location tuple.
    ///
    elle::Status	Riffle::Update(const path::Slab&	slab,
				       const nucleus::Location&	location)
    {
      std::pair<Riffle::Iterator, elle::Boolean>	result;
      Riffle::Scoutor					scoutor;

      enter();

      // try to look up the element in the current riffle.
      if ((scoutor = this->children.find(slab)) == this->children.end())
	{
	  Riffle*	riffle;

	  enter(instance(riffle));

	  // check that available slots remain i.e it is possible that
	  // the whole shrub's capacity is not large enough to hold
	  // all the the route's slabs.
	  if (Shrub::Timestamps.container.size() >=
	      Infinit::Configuration.shrub.capacity)
	    leave();

	  // allocate a new riffle.
	  riffle = new Riffle;

	  // create the riffle.
	  if (riffle->Create(slab, location, this) == elle::StatusError)
	    escape("unable to create the riffle");

	  // add the riffle to the queue.
	  if (Shrub::Timestamps.Add(riffle) == elle::StatusError)
	    escape("unable to add the riffle");

	  // insert it.
	  result =
	    this->children.insert(Riffle::Value(riffle->slab, riffle));

	  // check the result.
	  if (result.second == false)
	    escape("unable to insert the new riffle");

	  // stop tracking.
	  waive(riffle);

	  // release the resources.
	  release();
	}
      else
	{
	  Riffle*	riffle;

	  // retrive the pointer.
	  riffle = scoutor->second;

	  // if found, update it with the new address.
	  riffle->location = location;

	  // remove the riffle from the queue.
	  if (Shrub::Timestamps.Remove(riffle) == elle::StatusError)
	    escape("unable to remove the riffle");

	  // refresh the timestamp.
	  if (riffle->timestamp.Current() == elle::StatusError)
	    escape("unable to retrieve the current time");

	  // finally, add the riffle back to the queue i.e at its new position.
	  if (Shrub::Timestamps.Add(riffle) == elle::StatusError)
	    escape("unable to add the riffle");
	}

      leave();
    }

    ///
    /// this method destroys the entry associated with the given slab.
    ///
    elle::Status	Riffle::Destroy(const path::Slab&	slab)
    {
      Riffle::Iterator	iterator;
      Riffle*		riffle;

      enter();

      // try to look up the element in the current riffle.
      if ((iterator = this->children.find(slab)) == this->children.end())
	escape("unable to locate the given slab to destroy");

      // retrieve the riffle pointer.
      riffle = iterator->second;

      // clear the riffle.
      if (riffle->Clear() == elle::StatusError)
	escape("unable to clear the riffle");

      // release the shrub slot.
      if (Shrub::Timestamps.Remove(riffle) == elle::StatusError)
	escape("unable to remove the riffle");

      // delete the referenced riffle, along with its children.
      delete riffle;

      // remove the entry from the container.
      this->children.erase(iterator);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Riffle::Clear()
    {
      Riffle::Scoutor	scoutor;

      enter();

      // go through the children.
      for (scoutor = this->children.begin();
	   scoutor != this->children.end();
	   scoutor++)
	{
	  Riffle*	riffle = scoutor->second;

	  // clear the riffle recursively.
	  if (riffle->Clear() == elle::StatusError)
	    escape("unable to clear the riffle");

	  // release the shrub slot.
	  if (Shrub::Timestamps.Remove(riffle) == elle::StatusError)
	    escape("unable to remove the riffle");

	  // delete the riffle.
	  delete riffle;
	}

      // clear the container.
      this->children.clear();

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the riffle in a recursive way.
    ///
    elle::Status	Riffle::Dump(const elle::Natural32	margin)
    {
      elle::String	alignment(margin, ' ');
      Riffle::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Riffle] "
		<< std::hex << this << std::endl;

      // dump the attributes.
      std::cout << alignment << elle::Dumpable::Shift << "[Slab] "
		<< this->slab << std::endl;

      if (this->location.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the location");

      if (this->timestamp.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the timestamp");

      std::cout << alignment << elle::Dumpable::Shift << "[Parent] "
		<< std::hex << this->parent << std::endl;

      std::cout << alignment << elle::Dumpable::Shift
		<< "[Children]" << std::endl;

      // recursively dump all the sub-riffles.
      for (scoutor = this->children.begin();
	   scoutor != this->children.end();
	   scoutor++)
	{
	  // dump the sub-riffle.
	  if (scoutor->second->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the sub-riffle");
	}

      leave();
    }

  }
}
