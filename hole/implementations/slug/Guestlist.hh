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
	typedef std::pair<elle::TCPSocket*, Host*>	Value;
	typedef std::map<elle::TCPSocket*, Host*>	Container;
	typedef typename Container::iterator		Iterator;
	typedef typename Container::const_iterator	Scoutor;

	//
	// constructors & destructors
	//
	~Guestlist();

	//
	// methods
	//
	elle::Status		Add(elle::TCPSocket*,
				    Host*);
	elle::Status		Exist(elle::TCPSocket*) const;
	elle::Status		Retrieve(elle::TCPSocket*,
					 Host*&) const;
	elle::Status		Remove(elle::TCPSocket*);
	elle::Status		Locate(elle::TCPSocket*,
				       Scoutor&) const;
	elle::Status		Locate(elle::TCPSocket*,
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
