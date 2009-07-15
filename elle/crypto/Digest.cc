//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/crypto/Digest.cc
//
// created       julien quintard   [mon feb  2 22:22:12 2009]
// updated       julien quintard   [wed mar 11 16:03:52 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/crypto/Digest.hh>

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
    const String		Digest::Class = "Digest";

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method initializes the attributes.
    ///
    Status		Digest::New(Digest&)
    {
      leave();
    }

    ///
    /// this method releases the resources.
    ///
    Status		Digest::Delete(Digest&)
    {
      leave();
    }

    ///
    /// assign the given digest by duplicating the attributes.
    ///
    Digest&		Digest::operator=(const Digest&		element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // reinitialize the object.
      if ((Digest::Delete(*this) == StatusError) ||
	  (Digest::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      // set the region.
      this->region = element.region;

      return (*this);
    }

    ///
    /// this method check if two digests match.
    ///
    Boolean		Digest::operator==(const Digest&	element)
    {
      // compare the regions.
      return (this->region == element.region);
    }

    ///
    /// this method checks if two digests dis-match.
    ///
    Boolean		Digest::operator!=(const Digest&	element)
    {
      return (!(*this == element));
    }


//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the digest.
    ///
    Status		Digest::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[Digest]" << std::endl;

      if (this->region.Dump(margin + 2) == StatusError)
	escape("unable to dump the region");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a digest object.
    ///
    Status		Digest::Serialize(Archive&		archive) const
    {
      Archive		ar;

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Digest::Class) == StatusError)
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
    /// this method extracts a digest object.
    ///
    Status		Digest::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;

      // extract the digest archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the digest archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // check the name.
      if (Digest::Class != name)
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
  /// this operator renders a digest.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const elle::crypto::Digest&	element)
  {
    stream << element.region;

    return (stream);
  }

}
