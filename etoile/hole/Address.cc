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
// updated       julien quintard   [sun apr 18 16:25:17 2010]
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
      enter();

      // set the family.
      this->family = family;

      // release the previous digest.
      if (this->digest != NULL)
	delete this->digest;

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
      Archive		archive;

      enter();

      // check the address.
      if (this->digest == NULL)
	escape("unable to identify a null address");

      // create the archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the address.
      if (archive.Serialize(*this) == StatusError)
	escape("unable to serialize the address");

      // encode in base64.
      if (Base64::Encode(archive, string) == StatusError)
	escape("unable to encode the archive in base64");

      leave();      
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Address::operator==(const Address&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // if both are NULL or equal return true, false otherwise
      if ((this->digest == NULL) || (element.digest == NULL))
	{
	  if ((this->family != element.family) ||
	      (this->digest != element.digest))
	    false();
	}
      else
	{
	  if ((this->family != element.family) ||
	      (*this->digest != *element.digest))
	    false();
	}

      true();
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Address);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an address object.
    ///
    Status		Address::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Address]" << std::endl;

      if (this->digest != NULL)
	{
	  std::cout << alignment << Dumpable::Shift << "[Family] "
		    << std::hex << this->family << std::endl;

	  if (this->digest->Dump(margin + 2) == StatusError)
	    escape("unable to dump the digest");
	}
      else
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Digest] " << none << std::endl;
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
      enter();

      if (this->digest != NULL)
	{
	  // serialize the internal digest.
	  if (archive.Serialize((Natural8&)this->family,
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

      enter();

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
	  if (archive.Extract((Natural8&)this->family,
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
    elle::core::Boolean	operator<(const etoile::hole::Address&	lhs,
				  const etoile::hole::Address&	rhs)
    {
      enter();

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
