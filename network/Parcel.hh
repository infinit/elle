//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Parcel.hh
//
// created       julien quintard   [tue mar 16 12:04:48 2010]
// updated       julien quintard   [sun mar 21 16:39:14 2010]
//

#ifndef ELLE_NETWORK_PARCEL_HH
#define ELLE_NETWORK_PARCEL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Session.hh>
#include <elle/network/Header.hh>
#include <elle/network/Data.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is just a wrapper for packets freshly received.
    ///
    class Parcel
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
      // attributes
      //
      Session*		session;
      Header*		header;
      Data*		data;
    };

  }
}

#endif
