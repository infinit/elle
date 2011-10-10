//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien.quintard   [mon oct 10 17:16:56 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Handle.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Handle::operator==(const Handle&	element) const
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

      // compare the digests.
      if (*this->digest < *element.digest)
	true();

      false();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Address, _());


  }
}
