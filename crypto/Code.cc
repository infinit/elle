//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/Code.cc
//
// created       julien quintard   [mon feb  2 22:22:12 2009]
// updated       julien quintard   [tue aug  4 13:53:09 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/crypto/Code.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace crypto
  {

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the given code by duplicating the attributes.
    ///
    Code&		Code::operator=(const Code&		element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // recycle the code.
      if (this->Recycle<Code>(&element) == StatusError)
	yield("unable to recycle the code", *this);

      return (*this);
    }

    ///
    /// this method check if two codes match.
    ///
    Boolean		Code::operator==(const Code&	element) const
    {
      // compare the regions.
      return (this->region == element.region);
    }

    ///
    /// this method checks if two codes dis-match.
    ///
    Boolean		Code::operator!=(const Code&	element) const
    {
      return (!(*this == element));
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
      // extract the content.
      if (archive.Extract(this->region) == StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}
