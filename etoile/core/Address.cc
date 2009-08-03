//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Address.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [mon aug  3 21:03:59 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Address.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable can easily be used for comparing with invalid addresses.
    ///
    const Address		Address::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Address::Address():
      digest(NULL)
    {
    }

    ///
    /// this is the copy constructor.
    ///
    Address::Address(const Address&				address)
    {
      // copy the digest, if present.
      if (address.digest != NULL)
	{
	  this->digest = new Digest;

	  *this->digest = *address.digest;
	}
      else
	{
	  this->digest = NULL;
	}
    }

    ///
    /// this method reinitializes the object.
    ///
    Address::~Address()
    {
      // release the resources.
      if (this->digest != NULL)
	delete this->digest;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// create the address based on an object by serializing it before
    /// hashing it.
    ///
    Status		Address::Create(Archivable&		object)
    {
      // allocate the digest object.
      this->digest = new Digest;

      // compute the digest based on the object's archive.
      if (OneWay::Hash(object, *this->digest) == StatusError)
	escape("unable to hash the given object");

      leave();
    }

    ///
    /// this method transforms the internal address representation into
    /// a string.
    ///
    Status		Address::Identify(String&		string) const
    {
      Natural32			i;
      std::ostringstream	stream;

      if (this->digest != NULL)
	{
	  // transform the digest data into an hexadecimal string.
	  for (i = 0; i < this->digest->region.size; i++)
	    stream << std::nouppercase << std::hex
		   << (elle::core::Natural32)this->digest->region.contents[i];

	  // inject the string in the given argument.
	  string = stream.str();
	}

      leave();      
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the publickey.
    ///
    Address&		Address::operator=(const Address&	element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // recycle the address.
      if (this->Recycle<Address>(&element) == StatusError)
	yield("unable to recycle the address", *this);

      return (*this);
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Address::operator==(const Address&	element) const
    {
      // if both are NULL or equal return true, false otherwise
      if ((this->digest == NULL) || (element.digest == NULL))
	return (this->digest == element.digest);

      return (*this->digest == *element.digest);
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Address::operator!=(const Address&	element) const
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
	  std::cout << alignment << shift << "[Digest] " << none << std::endl;
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
      if (this->digest != NULL)
	{
	  // serialize the internal digest.
	  if (archive.Serialize(*this->digest) == StatusError)
	    escape("unable to serialize the digest");
	}
      else
	{
	  // serialize 'none'.
	  if (archive.Serialize(none) == StatusError)
	    escape("unable to serialize 'none'");
	}

      leave();
    }

    ///
    /// this method extracts the address object.
    ///
    Status		Address::Extract(Archive&		archive)
    {
      Archive::Type	type;

      // fetch the next element's type.
      if (archive.Fetch(type) == StatusError)
	escape("unable to fetch the next element's type");

      if (type == Archive::TypeNull)
	{
	  // nothing to do, keep the digest to NULL.
	  if (archive.Extract(none) == StatusError)
	    escape("unable to extract null");
	}
      else
	{
	  // allocate a digest.
	  this->digest = new Digest;

	  // extract the internal digest.
	  if (archive.Extract(*this->digest) == StatusError)
	    escape("unable to extract the digest");
	}

      leave();
    }

  }
}
