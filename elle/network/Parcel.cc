//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Parcel.cc
//
// created       julien quintard   [tue mar 16 12:08:40 2010]
// updated       julien quintard   [sun mar 21 16:40:21 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Parcel.hh>

namespace elle
{
  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Parcel::Parcel():
      session(new Session),
      header(new Header),
      data(new Data)
    {
    }

    ///
    /// specific constructor.
    ///
    Parcel::Parcel(Session*					session,
		   Header*					header,
		   Data*					data):
      session(session),
      header(header),
      data(data)
    {
    }

    ///
    /// destructor.
    ///
    Parcel::~Parcel()
    {
      // release the session.
      if (this->session != NULL)
	delete this->session;

      // release the header.
      if (this->header != NULL)
	delete this->header;

      // release the data.
      if (this->data != NULL)
	delete this->data;
    }

  }
}
