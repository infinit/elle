//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Agent.hh
//
// created       julien quintard   [thu mar 11 16:29:56 2010]
// updated       julien quintard   [fri may  6 13:33:00 2011]
//

#ifndef ETOILE_USER_AGENT_HH
#define ETOILE_USER_AGENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX ??? est-ce necessaire?
    ///
    class Client;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class acts on the behalf of a user by performing cryptographic
    /// operations.
    ///
    /// note that this class does not perform the cryptographic operations
    /// but rather forward the call to the agent component.
    ///
    /// this class derives elle::PrivateKey in order to exhibit the same
    /// interface but also to benefit from the variadic-template-based
    /// methods.
    ///
    /// then, this class overrides the cryptographic operations in order
    /// to forward the calls to the agent component.
    ///
    class Agent:
      public elle::PrivateKey
    {
    public:
      //
      // constants
      //
      static const elle::Natural32		Expiration;

      //
      // enumerations
      //
      enum State
	{
	  StateUnauthenticated,
	  StateAuthenticated
	};

      //
      // constructors & destructors
      //
      Agent();
      ~Agent();

      //
      // methods
      //
      elle::Status	Create(const elle::PublicKey&,
			       elle::Channel*);

      elle::Status	Authenticate();
      elle::Status	Destroy();

      elle::Status	Decrypt(const elle::Code&,
                                elle::Clear&) const;
      elle::Status	Sign(const elle::Plain&,
                             elle::Signature&) const;
      elle::Status	Encrypt(const elle::Plain&,
                                elle::Code&) const;
      elle::Status	Derive(const elle::Seed&,
                               elle::PublicKey&) const;

      //
      // callbacks
      //
      elle::Status	Timeout();
      elle::Status	Error(const elle::String&);

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      State		state;
      elle::Timer	timer;
      elle::Channel*	channel;

      elle::PublicKey	K;
    };

  }
}

#endif
