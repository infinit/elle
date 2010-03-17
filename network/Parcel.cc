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
// updated       julien quintard   [tue mar 16 21:53:55 2010]
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
      context(new Context),
      header(new Header),
      data(new Data)
    {
    }

    ///
    /// specific constructor.
    ///
    Parcel::Parcel(Context*					context,
		   Header*					header,
		   Data*					data):
      context(context),
      header(header),
      data(data)
    {
    }

    ///
    /// destructor.
    ///
    Parcel::~Parcel()
    {
      // release the context.
      if (this->context != NULL)
	delete this->context;

      // release the header.
      if (this->header != NULL)
	delete this->header;

      // release the data.
      if (this->data != NULL)
	delete this->data;
    }

  }
}
