//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue mar 16 12:08:40 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Parcel.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

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
    Parcel::Parcel(Session*                                     session,
                   Header*                                      header,
                   Data*                                        data):
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

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the parcel's content.
    ///
    Status              Parcel::Dump(const Natural32            margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Parcel] " << std::hex << this << std::endl;

      // dump the session.
      if (this->session != NULL)
        {
          if (this->session->Dump(margin + 2) == Status::Error)
            escape("unable to dump the session");
        }

      // dump the header.
      if (this->header != NULL)
        {
          if (this->header->Dump(margin + 2) == Status::Error)
            escape("unable to dump the header");
        }

      // dump the data.
      if (this->data != NULL)
        {
          std::cout << alignment << Dumpable::Shift << "[Data]" << std::endl;

          if (this->data->Dump(margin + 4) == Status::Error)
            escape("unable to dump the data");
        }

      return Status::Ok;
    }

  }
}
