//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Token.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [thu mar 25 17:43:26 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Token.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Token::Token():
      code(NULL)
    {
    }

    ///
    /// this method releases and reinitializes.
    ///
    Token::~Token()
    {
      // release the resource.
      if (this->code != NULL)
	delete this->code;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates or update the token.
    ///
    Status		Token::Update(SecretKey&	key,
				      PublicKey&	K)
    {
      enter();

      // release the resources in the case the token has already been
      // updated before.
      if (this->code != NULL)
	delete this->code;

      // allocate the code.
      this->code = new Code;

      // encrypt the given secret key with the given public key.
      /* XXX[je comprends pas comment ca peut etre un Plain???]
      if (K.Encrypt(key, this->code) == StatusError)
	escape("unable to encrypt the key");
      */

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		Token::Dump(Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Token]" << std::endl;

      if (this->code != NULL)
	{
	  if (this->code->Dump(margin + 2) == StatusError)
	    escape("unable to dump the code");
	}
      else
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Code] " << none << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    Status		Token::Serialize(Archive&	archive) const
    {
      enter();

      if (this->code != NULL)
	{
	  // serialize the internal code.
	  if (archive.Serialize(*this->code) == StatusError)
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
    /// this method extracts the block object.
    ///
    Status		Token::Extract(Archive&	archive)
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
	  this->code = new Code;

	  // extract the internal code.
	  if (archive.Extract(*this->code) == StatusError)
	    escape("unable to extract the code");
	}

      leave();
    }

  }
}
