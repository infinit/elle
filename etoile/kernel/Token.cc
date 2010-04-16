//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Token.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [fri apr 16 14:57:32 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Token.hh>

namespace etoile
{
  namespace kernel
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
	  this->code = new Code(*token.code);
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
    Status		Token::Update(const SecretKey&		key,
				      const PublicKey&		K)
    {
      enter();

      // delete the previous code.
      if (this->code != NULL)
	delete this->code;

      // if the secret key is null, reinitialize to the default null token.
      if (key == SecretKey::Null)
	{
	  this->code = NULL;
	}
      else
	{
	  // allocate a new code.
	  this->code = new Code;

	  // encrypt the given secret key with the given public key.
	  if (K.Encrypt(key, *this->code) == StatusError)
	    escape("unable to encrypt the key");
	}

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Token::operator==(const Token&		element) const
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
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Token);

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

      // dump the code.
      if (this->code != NULL)
	{
	  if (this->code->Dump(margin + 2) == StatusError)
	    escape("unable to dump the parent Code class");
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

      // serialize the code.
      if (this->code != NULL)
	{
	  if (archive.Serialize(*this->code) == StatusError)
	    escape("unable to serialize the code");
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
	  // nothing to do, keep the code to NULL.
	  if (archive.Extract(none) == StatusError)
	    escape("unable to extract null");
	}
      else
	{
	  // allocate a code.
	  this->code = new Code;

	  // extract the code.
	  if (archive.Extract(*this->code) == StatusError)
	    escape("unable to extract the code");
	}

      leave();
    }

  }
}
