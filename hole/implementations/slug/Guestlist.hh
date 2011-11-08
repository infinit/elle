//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien.quintard   [mon nov  7 12:36:26 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_SLUG_GUESTLIST_HH
#define HOLE_IMPLEMENTATIONS_SLUG_GUESTLIST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <hole/implementations/slug/Host.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Guestlist:
	public elle::Entity
      {
      public:
	//
	// types
	//
	typedef std::pair<elle::Gate*, Host*>		Value;
	typedef std::map<elle::Gate*, Host*>		Container;
	typedef typename Container::iterator		Iterator;
	typedef typename Container::const_iterator	Scoutor;

	//
	// constructors & destructors
	//
	~Guestlist();

	//
	// methods
	//
	elle::Status		Add(elle::Gate*,
				    Host*);
	elle::Status		Exist(elle::Gate*) const;
	elle::Status		Retrieve(elle::Gate*,
					 Host*&) const;
	elle::Status		Remove(elle::Gate*);
	elle::Status		Locate(elle::Gate*,
				       Scoutor&) const;
	elle::Status		Locate(elle::Gate*,
				       Iterator&);

	//
	// interfaces
	//

	// dumpable
	elle::Status		Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	Container		container;
      };

    }
  }
}

#endif
