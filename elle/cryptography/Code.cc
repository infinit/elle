//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/cryptography/Code.cc
//
// created       julien quintard   [mon feb  2 22:22:12 2009]
// updated       julien quintard   [sun feb 28 18:36:52 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/Code.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace cryptography
  {

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this method check if two codes match.
    ///
    Boolean		Code::operator==(const Code&	element) const
    {
      // compare the regions.
      return (this->region == element.region);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the code.
    ///
    Status		Code::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Code]" << std::endl;

      if (this->region.Dump(margin + 2) == StatusError)
	escape("unable to dump the region");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a code object.
    ///
    Status		Code::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the region.
      if (archive.Serialize(this->region) == StatusError)
	escape("unable to serialize the region");

      leave();
    }

    ///
    /// this method extracts a code object.
    ///
    Status		Code::Extract(Archive&		archive)
    {
      enter();

      // extract the content.
      if (archive.Extract(this->region) == StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}
