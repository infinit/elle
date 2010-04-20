//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/libraries/elle/cryptography/Code.cc
//
// created       julien quintard   [mon feb  2 22:22:12 2009]
// updated       julien quintard   [mon apr 19 19:43:11 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/Code.hh>

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
    /// this defines a null code.
    ///
    const Code			Code::Null;

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this method check if two codes match.
    ///
    Boolean		Code::operator==(const Code&	element) const
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
    embed(Entity, Code);

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

      // display depending on the value.
      if (*this == Code::Null)
	{
	  std::cout << alignment << "[Code] Null" << std::endl;
	}
      else
	{
	  std::cout << alignment << "[Code]" << std::endl;

	  // dump the region.
	  if (this->region.Dump(margin + 2) == StatusError)
	    escape("unable to dump the region");
	}

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
