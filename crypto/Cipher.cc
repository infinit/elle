//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/Cipher.cc
//
// created       julien quintard   [mon feb  2 22:22:12 2009]
// updated       julien quintard   [wed jul 29 14:11:50 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/crypto/Cipher.hh>

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
    const String		Cipher::Class = "Cipher";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the attributes.
    ///
    Cipher::Cipher()
    {
    }

    ///
    /// this method releases the resources.
    ///
    Cipher::~Cipher()
    {
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the given cipher by duplicating the attributes.
    ///
    Cipher&		Cipher::operator=(const Cipher&		element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // recycle the cipher.
      if (this->Recycle<Cipher>() == StatusError)
	yield("unable to recycle the cipher", *this);

      // set the region.
      this->region = element.region;

      return (*this);
    }

    ///
    /// this method check if two ciphers match.
    ///
    Boolean		Cipher::operator==(const Cipher&	element)
    {
      // compare the regions.
      return (this->region == element.region);
    }

    ///
    /// this method checks if two ciphers dis-match.
    ///
    Boolean		Cipher::operator!=(const Cipher&	element)
    {
      return (!(*this == element));
    }


//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the cipher.
    ///
    Status		Cipher::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[Cipher]" << std::endl;

      if (this->region.Dump(margin + 2) == StatusError)
	escape("unable to dump the region");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a cipher object.
    ///
    Status		Cipher::Serialize(Archive&		archive) const
    {
      Archive		ar;

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Cipher::Class) == StatusError)
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
    /// this method extracts a cipher object.
    ///
    Status		Cipher::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;

      // extract the cipher archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the cipher archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // check the name.
      if (Cipher::Class != name)
	escape("wrong class name in the extract object");

      // extract the content.
      if (ar.Extract(this->region) == StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}
