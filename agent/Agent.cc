//
// ---------- header ----------------------------------------------------------
//
// project       agent
//
// license       infinit
//
// file          /home/mycure/infinit/agent/Agent.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [fri jun 10 13:18:46 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <agent/Agent.hh>

namespace agent
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "agent";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the agent.
  ///
  elle::Status		Agent::Initialize()
  {
    enter();

    // nothing to do.

    leave();
  }

  ///
  /// this method cleans the agent.
  ///
  elle::Status		Agent::Clean()
  {
    enter();

    // nothing to do.

    leave();
  }

  ///
  /// this method is called whenever one needs to decrypt a code with
  /// the agent's private key.
  ///
  elle::Status		Agent::Decrypt(const elle::Code&	code,
				       elle::Clear&		clear)
  {
    enter();

    printf("[XXX] Agent::Decrypt()\n");

    // perform the cryptographic operation.
    if (Infinit::Identity.pair.k.Decrypt(code, clear) == elle::StatusError)
      escape("unable to perform the decryption");

    leave();
  }

  ///
  /// this method is triggered whenever one needs to perform a signature.
  ///
  elle::Status		Agent::Sign(const elle::Plain&		plain,
				    elle::Signature&		signature)
  {
    enter();

    printf("[XXX] Agent::Sign()\n");

    // perform the cryptographic operation.
    if (Infinit::Identity.pair.k.Sign(plain, signature) == elle::StatusError)
      escape("unable to perform the signature");

    leave();
  }

}
