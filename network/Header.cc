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
// updated       julien quintard   [tue feb 23 13:17:02 2010]
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
    Status		Header::Create(const Tag		tag,
				       const Natural32		size)
    {
      // set the attributes.
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
      // serialize the attributes.
      if (archive.Serialize(Header::Name,
			    (Natural32&)this->tag,
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

      // extract the attributes.
      if (archive.Extract(name,
			  (Natural32&)this->tag,
			  this->size) == StatusError)
	escape("unable to serialize the header attributes");

      // verify the name.
      if (Header::Name != name)
	escape("incorrect name identifier");

      leave();

    }

  }
}
