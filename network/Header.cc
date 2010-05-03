//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Header.cc
//
// created       julien quintard   [tue feb 23 00:18:03 2010]
// updated       julien quintard   [mon may  3 22:50:26 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Header.hh>

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
    const String		Header::Name = "Message";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Header::Header():
      tag(TagNone),
      size(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initialises the tag and size.
    ///
    Status		Header::Create(const Event&		event,
				       const Tag		tag,
				       const Natural32		size)
    {
      enter();

      // set the attributes.
      this->event = event;
      this->tag = tag;
      this->size = size;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Header, _(), _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the header.
    ///
    Status		Header::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Header] " << std::endl;

      // dump the event.
      if (this->event.Dump(margin + 2) == StatusError)
	escape("unable to dump the event");

      // dump the tag.
      std::cout << alignment << Dumpable::Shift
		<< "[Tag] " << this->tag << std::endl;

      // dump the size.
      std::cout << alignment << Dumpable::Shift
		<< "[Size] " << this->size << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the name, size and tag.
    ///
    Status		Header::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(Header::Name,
			    this->event,
			    this->tag,
			    this->size) == StatusError)
	escape("unable to serialize the header attributes");

      leave();
    }

    ///
    /// this method extracts the name, size and tag.
    ///
    Status		Header::Extract(Archive&		archive)
    {
      String		name;

      enter();

      // extract the attributes.
      if (archive.Extract(name,
			  this->event,
			  this->tag,
			  this->size) == StatusError)
	escape("unable to serialize the header attributes");

      // verify the name.
      if (Header::Name != name)
	escape("incorrect name event");

      leave();

    }

  }
}
