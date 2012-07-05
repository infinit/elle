#include <elle/network/Header.hh>

#include <elle/standalone/Log.hh>
#include <elle/standalone/Report.hh>

#include <iostream>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable defines the name that every message header will
    /// include.
    ///
    const String                Header::Name = "Message";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Header::Header()
      : tag(TagUnknown)
    {}

    Status
    Header::Create(const Tag tag)
    {
      this->tag = tag;
      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Header, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the header.
    ///
    Status              Header::Dump(const Natural32            margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Header] " << std::endl;

      // dump the tag.
      std::cout << alignment << io::Dumpable::Shift
                << "[Tag] " << this->tag << std::endl;

      return Status::Ok;
    }

  }
}
