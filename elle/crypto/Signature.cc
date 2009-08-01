//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/Signature.cc
//
// created       julien quintard   [mon feb  2 22:22:12 2009]
// updated       julien quintard   [sat aug  1 15:44:07 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/crypto/Signature.hh>

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
    /// assign the given signature by duplicating the attributes.
    ///
    Signature&		Signature::operator=(const Signature&	element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // recycle the signature.
      if (this->Recycle<Signature>(&element) == StatusError)
	yield("unable to recycle the signature", *this);

      return (*this);
    }

    ///
    /// this method check if two signatures match.
    ///
    Boolean		Signature::operator==(const Signature&	element)
    {
      // compare the regions.
      return (this->region == element.region);
    }

    ///
    /// this method checks if two signatures dis-match.
    ///
    Boolean		Signature::operator!=(const Signature&	element)
    {
      return (!(*this == element));
    }


//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the signature.
    ///
    Status		Signature::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[Signature]" << std::endl;

      if (this->region.Dump(margin + 2) == StatusError)
	escape("unable to dump the region");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a signature object.
    ///
    Status		Signature::Serialize(Archive&		archive) const
    {
      // serialize the region.
      if (archive.Serialize(this->region) == StatusError)
	escape("unable to serialize the region");

      leave();
    }

    ///
    /// this method extracts a signature object.
    ///
    Status		Signature::Extract(Archive&		archive)
    {
      // extract the content.
      if (archive.Extract(this->region) == StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}
