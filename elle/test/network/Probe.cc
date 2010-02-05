//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/Probe.cc
//
// created       julien quintard   [fri nov 27 20:07:49 2009]
// updated       julien quintard   [thu feb  4 02:01:19 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/Probe.hh>

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
    const Tag			Probe::Identifier = TagProbe;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// creates a probe by setting a name.
    ///
    Status		Probe::Create(const String&		name)
    {
      this->name = name;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the packet.
    ///
    Status		Probe::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Probe]" << std::endl;

      std::cout << alignment << shift << "[Tag] "
		<< Probe::Identifier << std::endl;

      std::cout << alignment << shift << "[Name] "
		<< this->name << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the probe packet.
    ///
    Status		Probe::Serialize(Archive&		archive) const
    {
      // serialize the identifier.
      if (archive.Serialize((Natural32&)Probe::Identifier) == StatusError)
	escape("unable to serialize the identifier");

      // serialize the attributes.
      if (archive.Serialize(this->name) == StatusError)
	escape("unable to serialize the probe");

      leave();
    }

    ///
    /// this method extracts the archive, building a probe packet.
    Status		Probe::Extract(Archive&		archive)
    {
      // extract the attributes.
      if (archive.Extract(this->name) == StatusError)
	escape("unable to extract the probe");

      leave();
    }

  }
}
