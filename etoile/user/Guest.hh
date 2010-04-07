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
// updated       julien quintard   [wed mar 31 00:18:07 2010]
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
      public Dumpable
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
      static const Natural32		Expiration;

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
      Status		Create(Channel*);
      Status		Detach();
      Status		Destroy();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Add(Guest*);
      static Status	Locate(Channel*,
			       Iterator&);
      static Status	Retrieve(Channel*,
				 Guest*&);
      static Status	Remove(Guest*);

      static Status	Show(const Natural32 = 0);

      //
      // callbacks
      //
      Status		Timeout();
      Status		Error(const String&);

      //
      // static attributes
      //
      static Container	Guests;

      //
      // attributes
      //
      Timer		timer;
      Options		options;
      Channel*		channel;
    };

  }
}

#endif
