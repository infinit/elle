#include <elle/network/Parcel.hh>
#include <elle/network/Header.hh>
#include <elle/network/Data.hh>

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
      header(new Header),
      data(new Data)
    {
    }

    ///
    /// specific constructor.
    ///
    Parcel::Parcel(Header*                                      header,
                   Data*                                        data):
      header(header),
      data(data)
    {
    }

    ///
    /// destructor.
    ///
    Parcel::~Parcel()
    {
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

      // dump the header.
      if (this->header != NULL)
        {
          if (this->header->Dump(margin + 2) == Status::Error)
            escape("unable to dump the header");
        }

      // dump the data.
      if (this->data != NULL)
        {
          std::cout << alignment << io::Dumpable::Shift << "[Data]" << std::endl;

          if (this->data->Dump(margin + 4) == Status::Error)
            escape("unable to dump the data");
        }

      return Status::Ok;
    }

  }
}
