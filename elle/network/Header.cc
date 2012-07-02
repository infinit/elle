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
    Header::Header():
      tag(TagUnknown),
      size(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initialises the tag and size.
    ///
    Status              Header::Create(const concurrency::Event& event,
                                       const Tag                tag,
                                       const Natural32          size)
    {
      // set the attributes.
      this->event = event;
      this->tag = tag;
      this->size = size;

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

      // dump the event.
      if (this->event.Dump(margin + 2) == Status::Error)
        escape("unable to dump the event");

      // dump the tag.
      std::cout << alignment << io::Dumpable::Shift
                << "[Tag] " << this->tag << std::endl;

      // dump the size.
      std::cout << alignment << io::Dumpable::Shift
                << "[Size] " << this->size << std::endl;

      return Status::Ok;
    }

  }
}
