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
// updated       julien quintard   [tue mar 16 23:49:31 2010]
//

#ifndef ELLE_NETWORK_PARCEL_HH
#define ELLE_NETWORK_PARCEL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Context.hh>
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
      Parcel(Context*,
	     Header*,
	     Data*);
      ~Parcel();

      //
      // attributes
      //
      Context*		context;
      Header*		header;
      Data*		data;
    };

  }
}

#endif
