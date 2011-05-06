//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Token.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [thu may  5 15:43:47 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Token.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definition ------------------------------------------------------
//

    ///
    /// this macro defines an empty token.
    ///
    const Token			Token::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor
    ///
    Token::Token():
      code(NULL)
    {
    }

    ///
    /// copy constructor.
    ///
    Token::Token(const Token&					token)
    {
      // copy the code, if present.
      if (token.code != NULL)
	{
	  // duplicate the code.
	  this->code = new elle::Code(*token.code);
	}
      else
	this->code = NULL;
    }

    ///
    /// destructor.
    ///
    Token::~Token()
    {
      // release the code.
      if (this->code != NULL)
	delete this->code;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates or update the token.
    ///
    elle::Status	Token::Update(const elle::SecretKey&	key,
				      const elle::PublicKey&	K)
    {
      enter();

      // delete the previous code.
      if (this->code != NULL)
	delete this->code;

      // if the secret key is null, reinitialize to the default null token.
      if (key == elle::SecretKey::Null)
	{
	  this->code = NULL;
	}
      else
	{
	  // allocate a new code.
	  this->code = new elle::Code;

	  // encrypt the given secret key with the given public key.
	  if (K.Encrypt(key, *this->code) == elle::StatusError)
	    escape("unable to encrypt the key");
	}

      leave();
    }

    ///
    /// this method extracts the secret key from the token.
    ///
    elle::Status	Token::Extract(const elle::PrivateKey&	k,
				       elle::SecretKey&		key) const
    {
      enter();

      // check the code.
      if (this->code == NULL)
	escape("unable to retrieve the key out of a null token");

      // decrypt the code.
      if (k.Decrypt(*this->code, key) == elle::StatusError)
	escape("unable to decrypt the token's content");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Token::operator==(const Token&		element) const
    {
      enter();

      // check if the objects are the same.
      if (this == &element)
	true();

      // compare the code.
      if ((this->code == NULL) || (element.code == NULL))
	{
	  if (this->code != element.code)
	    false();
	}
      else
	{
	  if (*this->code != *element.code)
	    false();
	}

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Token, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status	Token::Dump(elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Token] " << std::endl;

      // dump the code.
      if (this->code != NULL)
	{
	  if (this->code->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the parent Code class");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Code] " << elle::none << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    elle::Status	Token::Serialize(elle::Archive&		archive) const
    {
      enter();

      // serialize the code.
      if (this->code != NULL)
	{
	  if (archive.Serialize(*this->code) == elle::StatusError)
	    escape("unable to serialize the code");
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
    /// this method extracts the block object.
    ///
    elle::Status	Token::Extract(elle::Archive&		archive)
    {
      elle::Archive::Type	type;

      enter();

      // fetch the next element's type.
      if (archive.Fetch(type) == elle::StatusError)
	escape("unable to fetch the next element's type");

      if (type == elle::Archive::TypeNull)
	{
	  // nothing to do, keep the code to NULL.
	  if (archive.Extract(elle::none) == elle::StatusError)
	    escape("unable to extract null");
	}
      else
	{
	  // allocate a code.
	  this->code = new elle::Code;

	  // extract the code.
	  if (archive.Extract(*this->code) == elle::StatusError)
	    escape("unable to extract the code");
	}

      leave();
    }

  }
}
