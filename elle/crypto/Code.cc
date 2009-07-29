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
// updated       julien quintard   [wed jul 29 14:12:35 2009]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		Code::Class = "Code";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the attributes.
    ///
    Code::Code()
    {
    }

    ///
    /// this method releases the resources.
    ///
    Code::~Code()
    {
    }

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
      if (this->Recycle<Code>() == StatusError)
	yield("unable to recycle the code", *this);

      // set the region.
      this->region = element.region;

      return (*this);
    }

    ///
    /// this method check if two codes match.
    ///
    Boolean		Code::operator==(const Code&	element)
    {
      // compare the regions.
      return (this->region == element.region);
    }

    ///
    /// this method checks if two codes dis-match.
    ///
    Boolean		Code::operator!=(const Code&	element)
    {
      return (!(*this == element));
    }


//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the code.
    ///
    Status		Code::Dump(const Natural32		margin)
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
      Archive		ar;

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Code::Class) == StatusError)
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
    /// this method extracts a code object.
    ///
    Status		Code::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;

      // extract the code archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the code archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // check the name.
      if (Code::Class != name)
	escape("wrong class name in the extract object");

      // extract the content.
      if (ar.Extract(this->region) == StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}
