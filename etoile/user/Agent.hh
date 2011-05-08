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
// updated       julien quintard   [sat may  7 22:29:00 2011]
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
    /// this class implements an interface similar to elle::PrivateKey's
    /// in order to be usable as such in the nucleus.
    ///
    class Agent:
      public elle::Entity
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

      template <typename T,
		typename... TT>
      elle::Status	Decrypt(const elle::Code&,
				T&,
				TT&...) const;

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

      //
      // forward methods
      //

      ///
      /// this methods are required because the compiler, given an Archive
      /// object will call a template-based method instead of the Plain one.
      ///
      /// we do not want this especially because the template-based methods
      /// build archives and we are already receiving an archive.
      ///

      elle::Status	Decrypt(const elle::Code&	code,
				elle::Archive&		archive) const
      {
	return (this->Decrypt(code, (elle::Plain&)archive));
      }

      elle::Status	Sign(const elle::Archive&	archive,
			     elle::Signature&		signature) const
      {
	return (this->Sign((elle::Plain&)archive, signature));
      }

      elle::Status	Encrypt(const elle::Archive&	archive,
				elle::Code&		code) const
      {
	return (this->Encrypt((elle::Plain&)archive, code));
      }

      //
      // variadic templates
      //

      // sign
      template <typename T1>
      elle::Status	Sign(const T1&,
			     elle::Signature&) const;
      template <typename T1,
		typename T2>
      elle::Status	Sign(const T1&,
			     const T2&,
			     elle::Signature&) const;
      template <typename T1,
		typename T2,
		typename T3>
      elle::Status	Sign(const T1&,
			     const T2&,
			     const T3&,
			     elle::Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      elle::Status	Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     elle::Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      elle::Status	Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     elle::Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      elle::Status	Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     elle::Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      elle::Status	Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     elle::Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      elle::Status	Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     elle::Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      elle::Status	Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     const T9&,
			     elle::Signature&) const;

      // encrypt
      template <typename T1>
      elle::Status	Encrypt(const T1&,
				elle::Code&) const;
      template <typename T1,
		typename T2>
      elle::Status	Encrypt(const T1&,
				const T2&,
				elle::Code&) const;
      template <typename T1,
		typename T2,
		typename T3>
      elle::Status	Encrypt(const T1&,
				const T2&,
				const T3&,
				elle::Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      elle::Status	Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				elle::Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      elle::Status	Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				const T5&,
				elle::Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      elle::Status	Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				const T5&,
				const T6&,
				elle::Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      elle::Status	Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				const T5&,
				const T6&,
				const T7&,
				elle::Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      elle::Status	Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				const T5&,
				const T6&,
				const T7&,
				const T8&,
				elle::Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      elle::Status	Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				const T5&,
				const T6&,
				const T7&,
				const T8&,
				const T9&,
				elle::Code&) const;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/user/Agent.hxx>

#endif
