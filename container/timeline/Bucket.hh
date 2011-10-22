//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien.quintard   [fri oct 21 07:30:33 2011]
//

#ifndef ELLE_CONTAINER_TIMELINE_BUCKET_HH
#define ELLE_CONTAINER_TIMELINE_BUCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/idiom/Close.hh>
# include <vector>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace utility;

  namespace container
  {
    namespace timeline
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// this class represents a set of timeline objects grouped together
      /// because they have the exact same timestamp.
      ///
      /// note that a very simple data structure is being used in this
      /// class because no more than a dozen buckets should be grouped in
      /// the same bucket.
      ///
      template <typename T>
      class Bucket:
	public Entity
      {
      public:
	//
	// types
	//
	typedef std::vector<T>				Container;
	typedef typename Container::iterator		Iterator;
	typedef typename Container::const_iterator	Scoutor;

	//
	// methods
	//
	Status		Add(T&);
	Status		Remove(T&);

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

#include <elle/container/timeline/Bucket.hxx>

#endif
