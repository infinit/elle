//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien.quintard   [fri oct 21 07:43:32 2011]
//

#ifndef ELLE_CONTAINER_TIMELINE_TIMELINE_HXX
#define ELLE_CONTAINER_TIMELINE_TIMELINE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  using namespace standalone;

  namespace container
  {
    namespace timeline
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor
      ///
      template <typename T>
      Timeline<T>::Timeline()
      {
      }

      ///
      /// destructor.
      ///
      template <typename T>
      Timeline<T>::~Timeline()
      {
	typename Timeline<T>::Scoutor	scoutor;

	// go through the container.
	for (scoutor = this->container.begin();
	     scoutor != this->container.end();
	     scoutor++)
	  {
	    // delete the bucket.
	    delete scoutor->second;
	  }

	// clear the container.
	this->container.clear();
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// this method insert an object in the timeline container.
      ///
      template <typename T>
      Status		Timeline<T>::Insert(const Time&		timestamp,
					    T&			object)
      {
	typename Timeline<T>::Iterator	iterator;
	Bucket<T>*			bucket;

	enter();

	// try to look up an bucket for the given timestamp.
	if ((iterator =
	       this->container.find(timestamp)) == this->container.end())
	  {
	    std::pair<typename Timeline<T>::Iterator, Boolean>	result;
	    Bucket<T>*						b;

	    enterx(instance(b));

	    // allocate an bucket.
	    b = new Bucket<T>;

	    // record the bucket with its timestamp.
	    result =
	      this->container.insert(
	        typename Timeline<T>::Value(timestamp, b));

	    // check the result.
	    if (result.second == false)
	      escape("unable to insert the object");

	    // set the bucket since _b_ is going to be reset to NULL.
	    bucket = b;

	    // waive.
	    waive(b);

	    release();
	  }
	else
	  {
	    // retrieve the bucket.
	    bucket = iterator->second;
	  }

	// add the object to the bucket.
	if (bucket->Add(object) == StatusError)
	  escape("unable to add the object to the bucket");

	leave();
      }

      ///
      /// this method deletes an object from the timeline.
      ///
      template <typename T>
      Status		Timeline<T>::Delete(const Time&		timestamp,
					    T&			object)
      {
	typename Timeline<T>::Iterator	iterator;
	Bucket<T>*			bucket;

	enter();

	// try to look up the bucket in the container.
	if ((iterator =
	       this->container.find(timestamp)) == this->container.end())
	  escape("unable to locate the given timestamp");

	// retrieve the bucket.
	bucket = iterator->second;

	// remove the object from the bucket.
	if (bucket->Remove(object) == StatusError)
	  escape("unable to remove the object");

	// check if the bucket is empty.
	if (bucket->container.empty() == true)
	  {
	    // delete the bucket.
	    delete bucket;

	    // remove the bucket from the container.
	    this->container.erase(iterator);
	  }

	leave();
      }

      ///
      /// this method clears the timeline container.
      ///
      template <typename T>
      Status		Timeline<T>::Clear()
      {
	enter();

	// clear the container.
	this->container.clear();

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the timeline container.
      ///
      template <typename T>
      Status		Timeline<T>::Dump(const Natural32	margin) const
      {
	String				alignment(margin, ' ');
	typename Timeline<T>::Scoutor	scoutor;

	enter();

	std::cout << alignment << "[Timeline] "
		  << std::dec << this->container.size() << std::endl;

	// go through the container.
	for (scoutor = this->container.begin();
	     scoutor != this->container.end();
	     scoutor++)
	  {
	    // dump the timestamp.
	    if (scoutor->first.Dump(margin + 2) == StatusError)
	      escape("unable to dump the timestamp");

	    // dump the bucket.
	    if (scoutor->second->Dump(margin + 2) == StatusError)
	      escape("unable to dump the bucket");
	  }

	leave();
      }

    }
  }
}

#endif
