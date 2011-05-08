//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Token.hxx
//
// created       julien quintard   [sat may  7 22:09:20 2011]
// updated       julien quintard   [sat may  7 22:15:55 2011]
//

#ifndef NUCLEUS_NEUTRON_TOKEN_HXX
#define NUCLEUS_NEUTRON_TOKEN_HXX

namespace nucleus
{
  namespace neutron
  {

//
// ---------- templates -------------------------------------------------------
//

    ///
    /// this method extracts the secret key from the token.
    ///
    /// note that this method takes an undefined type because one may want
    /// to extract a token directly with a private key while another may
    /// want to rely on an external agent for instance.
    ///
    template <typename T>
    elle::Status	Token::Extract(const T&			broker,
				       elle::SecretKey&		key) const
    {
      enter();

      // check the code.
      if (this->code == NULL)
	escape("unable to retrieve the key out of a null token");

      // decrypt the code.
      if (broker.Decrypt(*this->code, key) == elle::StatusError)
	escape("unable to decrypt the token's content");

      leave();
    }

  }
}

#endif
