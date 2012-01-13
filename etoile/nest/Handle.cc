//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon oct 17 10:07:47 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/nest/Handle.hh>

namespace etoile
{
  namespace nest
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Handle::Handle():
      state(StateUnloaded)
    {
      // this constructor should never be used.
      yield(_(), "this constructor should have never heen called");
    }

    ///
    /// address-specific constructor.
    ///
    Handle::Handle(const nucleus::Address&			address):
      state(StateUnloaded),
      address(address)
    {
    }

    ///
    /// placement-specific constructor.
    ///
    Handle::Handle(const Placement&				placement):
      state(StateUnloaded),
      placement(placement)
    {
    }

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

      // depending on the states.
      if ((this->state == Handle::StateLoaded) &&
	  (element.state == Handle::StateLoaded))
	{
	  //
	  // if both handles are loaded, compare the placements.
	  //

	  // compare the placements.
	  if (this->placement != element.placement)
	    false();
	}
      else if ((this->state == Handle::StateUnloaded) &&
	       (element.state == Handle::StateUnloaded))
	{
	  // compare the addresses.
	  if (this->address != Address
	}

      // if both addresses are different from null/some...
      if (this->address !=
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
