//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu oct 20 17:52:03 2011]
//

#ifndef ELLE_CONTAINER_TIMELINE_TIMELINE_HH
#define ELLE_CONTAINER_TIMELINE_TIMELINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/utility/Time.hh>

#include <elle/container/timeline/Bucket.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace utility;

  namespace container
  {
    ///
    /// this namespace contains all the classes related to the timeline
    /// container.
    ///
    namespace timeline
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// this container makes it easy to store objects according to a
      /// timestamp.
      ///
      /// this construct is therefore heavily used when it comes to
      /// implementing cache policies for instance.
      ///
      template <typename T>
      class Timeline:
	public Entity
      {
      public:
	//
	// types
	//
	typedef std::pair<const Time, Bucket<T>*>	Value;
	typedef std::map<const Time, Bucket<T>*>	Container;
	typedef typename Container::iterator		Iterator;
	typedef typename Container::const_iterator	Scoutor;

	//
	// constructors & destructors
	//
	Timeline();
	~Timeline();

	//
	// methods
	//
	Status		Insert(const Time&,
			       T&);
	Status		Delete(const Time&,
			       T&);

	Status		Clear();

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	Container	container;
      };

    }
  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/container/timeline/Timeline.hxx>

#endif
