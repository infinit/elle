//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/door/Echo.cc
//
// created       julien quintard   [fri nov 27 20:07:49 2009]
// updated       julien quintard   [sun feb  7 01:36:53 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/door/Echo.hh>

namespace elle
{
  using namespace core;
  using namespace archive;
  using namespace network;

  namespace test
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the tag definition.
    ///
    const Tag			Echo::Identifier = TagEcho;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// creates a echo by setting a text.
    ///
    Status		Echo::Create(const String&		text)
    {
      this->text = text;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the packet.
    ///
    Status		Echo::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Echo]" << std::endl;

      std::cout << alignment << shift << "[Tag] "
		<< Echo::Identifier << std::endl;

      std::cout << alignment << shift << "[Text] "
		<< this->text << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the echo packet.
    ///
    Status		Echo::Serialize(Archive&		archive) const
    {
      // serialize the identifier.
      if (archive.Serialize((Natural32&)Echo::Identifier) == StatusError)
	escape("unable to serialize the identifier");

      // serialize the attributes.
      if (archive.Serialize(this->text) == StatusError)
	escape("unable to serialize the echo");

      leave();
    }

    ///
    /// this method extracts the archive, building a echo packet.
    Status		Echo::Extract(Archive&		archive)
    {
      // extract the attributes.
      if (archive.Extract(this->text) == StatusError)
	escape("unable to extract the echo");

      leave();
    }

  }
}
