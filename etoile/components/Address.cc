//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /data/mycure/repositories/infinit/core/components/Address.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [thu jul 16 11:38:13 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Address.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		Address::Class = "Address";

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// create the address based on an object by serializing it before
    /// hashing it.
    ///
    Status		Address::Create(Archivable&		object)
    {
      Archive		archive;

      // create a temporary archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the given object.
      if (archive.Serialize(object) == StatusError)
	escape("unable to serialize the given object");

      // allocate the digest object.
      this->digest = new Digest;

      // compute the digest based on the object's archive.
      if (OneWay::Hash(archive, *this->digest) == StatusError)
	escape("unable to hash the given object's archive");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Status		Address::New(Address&			address)
    {
      address.digest = NULL;

      leave();
    }

    ///
    /// this method reinitializes the object.
    ///
    Status		Address::Delete(Address&		address)
    {
      // release the resources.
      if (address.digest != NULL)
	delete address.digest;

      leave();
    }

    ///
    /// assign the publickey.
    ///
    Address&		Address::operator=(const Address&	element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // reinitialize the object.
      if ((Address::Delete(*this) == StatusError) ||
	  (Address::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      // copy the digest, if present.
      if (element.digest != NULL)
	{
	  this->digest = new Digest;

	  *this->digest = *element.digest;
	}

      return (*this);
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Address::operator==(const Address&	element)
    {
      // if one of the two are NULL, return true if both are NULL
      // or false otherwise.
      if ((this->digest == NULL) || (element.digest == NULL))
	return (this->digest == element.digest);

      return (*this->digest == *element.digest);
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Address::operator!=(const Address&	element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an address object.
    ///
    Status		Address::Dump(Natural32			margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Address]" << std::endl;

      if (this->digest != NULL)
	{
	  if (this->digest->Dump(margin + 2) == StatusError)
	    escape("unable to dump the digest");
	}
      else
	{
	  std::cout << alignment << shift << none << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the address object.
    ///
    Status		Address::Serialize(Archive&		archive) const
    {
      Archive		ar;

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Address::Class) == StatusError)
	escape("unable to serialize the class name");

      if (this->digest != NULL)
	{
	  // serialize the internal digest.
	  if (ar.Serialize(*this->digest) == StatusError)
	    escape("unable to serialize the digest");
	}
      else
	{
	  // serialize 'none'.
	  if (ar.Serialize(none) == StatusError)
	    escape("unable to serialize 'none'");
	}

      // record in the archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object's archive");

      leave();
    }

    ///
    /// this method extracts the address object.
    ///
    Status		Address::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;
      Archive::Type	type;

      // extract the address archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the address archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // check the name.
      if (Address::Class != name)
	escape("wrong class name in the extract object");

      // fetch the next element's type.
      if (ar.Fetch(type) == StatusError)
	escape("unable to fetch the next element's type");

      if (type == Archive::TypeNull)
	{
	  // nothing to do, keep the digest to NULL.
	}
      else
	{
	  // allocate a digest.
	  this->digest = new Digest;

	  // extract the internal digest.
	  if (ar.Extract(*this->digest) == StatusError)
	    escape("unable to extract the digest");
	}

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
  /// this function displays an address.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const etoile::components::Address& element)
  {
    if (element.digest != NULL)
      stream << *element.digest;
    else
      stream << none;

    return (stream);
  }

}
