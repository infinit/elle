//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Address.hxx
//
// created       julien quintard   [sun may  8 00:10:37 2011]
// updated       julien quintard   [sun may  8 09:26:24 2011]
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
    elle::Status	Address::Create(const T&		parameter,
					const TT&...		parameters)
    {
      enter();

      // release the previous digest.
      if (this->digest != NULL)
	delete this->digest;

      // allocate the digest object.
      this->digest = new elle::Digest;

      // compute the digest based on the parameters.
      if (elle::OneWay::Hash(parameter, parameters...,
			     *this->digest) == elle::StatusError)
	escape("unable to hash the given parameter(s)");

      leave();
    }

  }
}

#endif
