//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Guest.hh
//
// created       julien quintard   [wed mar 17 21:49:41 2010]
// updated       julien quintard   [mon may  3 13:46:16 2010]
//

#ifndef ETOILE_USER_GUEST_HH
#define ETOILE_USER_GUEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an unconnected/unauthentified connection.
    ///
    /// such a connection will be destroyed if it does not connect/authentify
    /// within a time frame.
    ///
    class Guest:
      public elle::Entity
    {
    public:
      //
      // enumerations
      //
      enum Options
	{
	  OptionNone,
	  OptionDetach
	};

      //
      // constants
      //
      static const elle::Natural32		Expiration;

      //
      // types
      //
      typedef std::list<Guest*>			Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      Guest();
      ~Guest();

      //
      // methods
      //
      elle::Status		Create(elle::Channel*);
      elle::Status		Detach();
      elle::Status		Destroy();

      //
      // interfaces
      //

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Add(Guest*);
      static elle::Status	Locate(elle::Channel*,
				       Iterator&);
      static elle::Status	Retrieve(elle::Channel*,
					 Guest*&);
      static elle::Status	Remove(Guest*);

      static elle::Status	Show(const elle::Natural32 = 0);

      //
      // callbacks
      //
      elle::Status		Timeout();
      elle::Status		Error(const elle::String&);

      //
      // static attributes
      //
      static Container	Guests;

      //
      // attributes
      //
      elle::Timer	timer;
      Options		options;
      elle::Channel*	channel;
    };

  }
}

#endif
