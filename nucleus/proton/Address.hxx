//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sun may  8 00:10:37 2011]
//

#ifndef NUCLEUS_PROTON_ADDRESS_HXX
#define NUCLEUS_PROTON_ADDRESS_HXX

namespace nucleus
{
  namespace proton
  {

//
// ---------- templates -------------------------------------------------------
//

    ///
    /// create the address based on an object by serializing it before
    /// hashing it.
    ///
    template <typename T,
	      typename... TT>
    elle::Status	Address::Create(const Family&		family,
					const
					  neutron::Component&	component,
					const T&		parameter,
					const TT&...		parameters)
    {
      enter();

      // set the family.
      this->family = family;

      // set the component.
      this->component = component;

      // release the previous digest.
      if (this->digest != NULL)
	delete this->digest;

      // allocate the digest object.
      this->digest = new elle::Digest;

      // compute the digest based on the parameters. note that most of
      // the components requesting this method pass family and component
      // in the parameters as well. for examples, please refer to
      // ContentHashBlock, PublicKeyBlock etc.
      if (elle::OneWay::Hash(parameter, parameters...,
			     *this->digest) == elle::StatusError)
	escape("unable to hash the given parameter(s)");

      leave();
    }

  }
}

#endif
