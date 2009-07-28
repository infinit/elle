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
// updated       julien quintard   [mon jul 27 08:37:41 2009]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		Signature::Class = "Signature";

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this method initializes the attributes.
    ///
    Status		Signature::New(Signature&)
    {
      leave();
    }

    ///
    /// this method releases the resources.
    ///
    Status		Signature::Delete(Signature&)
    {
      leave();
    }

    ///
    /// assign the given signature by duplicating the attributes.
    ///
    Signature&		Signature::operator=(const Signature&	element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // reinitialize the object.
      if ((Signature::Delete(*this) == StatusError) ||
	  (Signature::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      // set the region.
      this->region = element.region;

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
      Archive		ar;

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Signature::Class) == StatusError)
	escape("unable to serialize the class name");

      // serialize the region.
      if (ar.Serialize(this->region) == StatusError)
	escape("unable to serialize the region");

      // record the object's archive into the given archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object archive");

      leave();
    }

    ///
    /// this method extracts a signature object.
    ///
    Status		Signature::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;

      // extract the signature archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the signature archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // check the name.
      if (Signature::Class != name)
	escape("wrong class name in the extract object");

      // extract the content.
      if (ar.Extract(this->region) == StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this operator renders a signature.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const elle::crypto::Signature& element)
  {
    stream << element.region;

    return (stream);
  }

}
