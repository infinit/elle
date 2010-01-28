//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/hole/Address.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [thu jan 28 00:42:36 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Address.hh>

namespace etoile
{
  namespace hole
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
      family(FamilyUnknown),
      digest(NULL)
    {
    }

    ///
    /// this is the copy constructor.
    ///
    Address::Address(const Address&				address):
      Entity::Entity(address)
    {
      // copy the digest, if present.
      if (address.digest != NULL)
	{
	  this->family = address.family;

	  this->digest = new Digest;
	  *this->digest = *address.digest;
	}
      else
	{
	  this->family = FamilyUnknown;
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
    Status		Address::Create(const Family		family,
					const Archivable&	object)
    {
      // set the family.
      this->family = family;

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
	  // transform the family into an hexadicemal sequence.
	  stream << std::nouppercase << std::hex
		 << (elle::core::Natural32)this->family;

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
    /// assign the address.
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
	return ((this->family == element.family) &&
		(this->digest == element.digest));

      return ((this->family == element.family) &&
	      (*this->digest == *element.digest));
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
    Status		Address::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Address]" << std::endl;

      if (this->digest != NULL)
	{
	  std::cout << alignment << shift << "[Family]"
		    << std::hex << this->family << std::endl;

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
	  if (archive.Serialize((Byte&)this->family,
				*this->digest) == StatusError)
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
	  if (archive.Extract((Byte&)this->family,
			      *this->digest) == StatusError)
	    escape("unable to extract the digest");
	}

      leave();
    }

//
// ---------- operators -------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	operator<(const etoile::hole::Address&	lhs,
				  const etoile::hole::Address&	rhs)
    {
      // test for a null digest.
      if ((lhs.digest == NULL) && (rhs.digest == NULL))
	false();
      else if (lhs.digest == NULL)
	true();
      else if (rhs.digest == NULL)
	false();
      else
	{
	  Natural32		i;

	  // test the sizes.
	  if (lhs.digest->region.size < rhs.digest->region.size)
	    true();
	  else if (lhs.digest->region.size > rhs.digest->region.size)
	    false();

	  // compare the family.
	  if (lhs.family < rhs.family)
	    true();

	  // finally, go through the data and compare.
	  for (i = 0; i < lhs.digest->region.size; i++)
	    {
	      // if the elements are different.
	      if (lhs.digest->region.contents[i] !=
		  rhs.digest->region.contents[i])
		{
		  return (lhs.digest->region.contents[i] <
			  rhs.digest->region.contents[i] ?
			  StatusTrue : StatusFalse);
		}
	    }
	}

      false();
    }

  }
}
