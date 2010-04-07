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
// updated       julien quintard   [tue apr  6 21:48:22 2010]
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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates or update the token.
    ///
    Status		Token::Update(const SecretKey&		key,
				      const PublicKey&		K)
    {
      enter();

      printf("[XXX] Token::Update() BROKEN\n");

      // encrypt the given secret key with the given public key.
      /* XXX[je comprends pas comment ca peut etre un Plain???]
      if (K.Encrypt(key, *this->code) == StatusError)
	escape("unable to encrypt the key");
      */

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
      return (Code::operator==(element));
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

      // dump the parent class.
      if (Code::Dump(margin + 2) == StatusError)
	escape("unable to dump the parent Code class");

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
      return (Code::Serialize(archive));
    }

    ///
    /// this method extracts the block object.
    ///
    Status		Token::Extract(Archive&	archive)
    {
      return (Code::Extract(archive));
    }

  }
}
