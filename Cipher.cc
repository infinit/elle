//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/cryptography/Cipher.cc
//
// created       julien quintard   [mon feb  2 22:22:12 2009]
// updated       julien quintard   [sun apr 18 12:38:17 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/Cipher.hh>

namespace elle
{
  using namespace core;
  using namespace miscellaneous;
  using namespace archive;

  namespace cryptography
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines a null cipher.
    ///
    const Cipher		Cipher::Null;

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this method check if two ciphers match.
    ///
    Boolean		Cipher::operator==(const Cipher&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the regions.
      if (this->region != element.region)
	false();

      true();
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Cipher);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the cipher.
    ///
    Status		Cipher::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

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
      enter();

      // serialize the region.
      if (archive.Serialize(this->region) == StatusError)
	escape("unable to serialize the region");

      leave();
    }

    ///
    /// this method extracts a cipher object.
    ///
    Status		Cipher::Extract(Archive&		archive)
    {
      enter();

      // extract the content.
      if (archive.Extract(this->region) == StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}
