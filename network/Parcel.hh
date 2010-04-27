//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/network/Parcel.hh
//
// created       julien quintard   [tue mar 16 12:04:48 2010]
// updated       julien quintard   [mon apr 26 19:08:07 2010]
//

#ifndef ELLE_NETWORK_PARCEL_HH
#define ELLE_NETWORK_PARCEL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Meta.hh>

#include <elle/network/Session.hh>
#include <elle/network/Header.hh>
#include <elle/network/Data.hh>

namespace elle
{
  namespace network
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Session;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is just a wrapper for packets freshly received.
    ///
    class Parcel:
      public Meta,
      public Dumpable
    {
    public:
      //
      // constructors & destructors
      //
      Parcel();
      Parcel(Session*,
	     Header*,
	     Data*);
      ~Parcel();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Session*		session;
      Header*		header;
      Data*		data;
    };

  }
}

#endif
