//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Agent.cc
//
// created       julien quintard   [thu mar 11 17:01:29 2010]
// updated       julien quintard   [thu mar 11 17:22:33 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Agent.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Agent::Agent(const PublicKey&				K):
      K(K),
      socket(NULL)
    {
    }

    Agent::~Agent()
    {
      // delete the socket, if present.
      if (this->socket != NULL)
	delete this->socket;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method decrypts a code by requesting the agent process
    /// to perform the cryptographic operation.
    ///
    Status		Agent::Decrypt(const Code&		code,
				       Clear&			clear) const
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// this method signs a code by requesting the agent process
    /// to perform the cryptographic operation.
    ///
    Status		Agent::Sign(const Plain&		plain,
				    Signature&			signature)
      const
    {
      enter();

      // XXX

      leave();
    }

  }
}
