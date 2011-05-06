//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Address.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [thu may  5 13:42:43 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Address.hh>

namespace nucleus
{
  namespace proton
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
	  this->digest = new elle::Digest(*address.digest);
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
    elle::Status	Address::Create(const elle::Archivable&	object)
    {
      enter();

      // release the previous digest.
      if (this->digest != NULL)
	delete this->digest;

      // allocate the digest object.
      this->digest = new elle::Digest;

      // compute the digest based on the object's archive.
      if (elle::OneWay::Hash(object, *this->digest) == elle::StatusError)
	escape("unable to hash the given object");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Address::operator==(const Address&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // if both are NULL or equal return true, false otherwise
      if ((this->digest == NULL) || (element.digest == NULL))
	{
	  if (this->digest != element.digest)
	    false();
	}
      else
	{
	  if (*this->digest != *element.digest)
	    false();
	}

      true();
    }

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Address::operator<(const Address&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	false();

      // test for a null digest.
      if ((this->digest == NULL) && (element.digest == NULL))
	false();
      else if (this->digest == NULL)
	true();
      else if (element.digest == NULL)
	false();
      else
	{
	  elle::Natural32	i;

	  // test the sizes.
	  if (this->digest->region.size < element.digest->region.size)
	    true();
	  else if (this->digest->region.size > element.digest->region.size)
	    false();

	  // finally, go through the data and compare.
	  for (i = 0; i < this->digest->region.size; i++)
	    {
	      // if the elements are different.
	      if (this->digest->region.contents[i] !=
		  element.digest->region.contents[i])
		{
		  return (this->digest->region.contents[i] <
			  element.digest->region.contents[i] ?
			  elle::StatusTrue : elle::StatusFalse);
		}
	    }
	}

      false();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Address, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an address object.
    ///
    elle::Status	Address::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      // display the address depending on its value.
      if (*this == Address::Null)
	{
	  std::cout << alignment << "[Address] " << elle::none << std::endl;
	}
      else
	{
	  std::cout << alignment << "[Address] " << std::endl;

	  // dump the digest.
	  if (this->digest->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the digest");
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the address object.
    ///
    elle::Status	Address::Serialize(elle::Archive&	archive) const
    {
      enter();

      if (this->digest != NULL)
	{
	  // serialize the internal digest.
	  if (archive.Serialize(*this->digest) == elle::StatusError)
	    escape("unable to serialize the digest");
	}
      else
	{
	  // serialize 'none'.
	  if (archive.Serialize(elle::none) == elle::StatusError)
	    escape("unable to serialize 'none'");
	}

      leave();
    }

    ///
    /// this method extracts the address object.
    ///
    elle::Status	Address::Extract(elle::Archive&		archive)
    {
      elle::Archive::Type	type;

      enter();

      // fetch the next element's type.
      if (archive.Fetch(type) == elle::StatusError)
	escape("unable to fetch the next element's type");

      if (type == elle::Archive::TypeNull)
	{
	  // nothing to do, keep the digest to NULL.
	  if (archive.Extract(elle::none) == elle::StatusError)
	    escape("unable to extract null");
	}
      else
	{
	  // allocate a digest.
	  this->digest = new elle::Digest;

	  // extract the internal digest.
	  if (archive.Extract(*this->digest) == elle::StatusError)
	    escape("unable to extract the digest");
	}

      leave();
    }

  }
}
