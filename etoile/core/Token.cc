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
// updated       julien quintard   [wed jul 29 17:04:56 2009]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		Token::Class = "Token";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Token::Token()
    {
      this->code = NULL;
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
    Status		Token::Dump(Natural32		margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Token]" << std::endl;

      if (this->code != NULL)
	{
	  if (this->code->Dump(margin + 2) == StatusError)
	    escape("unable to dump the code");
	}
      else
	{
	  std::cout << alignment << shift << "[Code] " << none << std::endl;
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
      Archive		ar;

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Token::Class) == StatusError)
	escape("unable to serialize the class name");

      if (this->code != NULL)
	{
	  // serialize the internal code.
	  if (ar.Serialize(*this->code) == StatusError)
	    escape("unable to serialize the digest");
	}
      else
	{
	  // serialize 'none'.
	  if (ar.Serialize(none) == StatusError)
	    escape("unable to serialize 'none'");
	}

      // record in the archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object's archive");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		Token::Extract(Archive&	archive)
    {
      Archive		ar;
      String		name;
      Archive::Type	type;

      // extract the token archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the token archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // check the name.
      if (Token::Class != name)
	escape("wrong class name in the extract object");

      // fetch the next element's type.
      if (ar.Fetch(type) == StatusError)
	escape("unable to fetch the next element's type");

      if (type == Archive::TypeNull)
	{
	  // nothing to do, keep the code to NULL.
	}
      else
	{
	  // allocate a digest.
	  this->code = new Code;

	  // extract the internal code.
	  if (ar.Extract(*this->code) == StatusError)
	    escape("unable to extract the code");
	}

      leave();
    }

  }
}
