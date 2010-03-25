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
// updated       julien quintard   [tue mar 23 21:05:54 2010]
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
