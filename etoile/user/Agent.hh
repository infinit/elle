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
// updated       julien quintard   [mon may  3 13:37:43 2010]
//

#ifndef ETOILE_USER_AGENT_HH
#define ETOILE_USER_AGENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/Manifest.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Client;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class acts on the behalf of a user by performing cryptographic
    /// operations or, whenever unable, requesting the agent to do it.
    ///
    /// note that the Decrypt() and Sign() methods are virtual so
    /// that an application using the Etoile library can overwrite these
    /// methods, to prevent network communication with the Agent for instance.
    ///
    /// XXX deux type (internal, external) pour debug notamment mais aussi
    /// pour les perfs ou pour initializer des choses.
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
      enum Type
	{
	  TypeUnknown,

	  TypeLocal,
	  TypeRemote
	};

      enum State
	{
	  StateUnauthenticated,
	  StateAuthenticated
	};

      //
      // class
      //

      ///
      /// the local class only contains the private key for
      /// performing sensitive operations.
      ///
      class Local:
	public elle::Entity
      {
      public:
	//
	// attributes
	//
	elle::PrivateKey	k;

	//
	// interfaces
	//

	// dumpable
	elle::Status	Dump(const elle::Natural32 = 0) const;
      };

      ///
      /// the remote class contains everything for communicating
      /// with an external agent process.
      ///
      struct Remote:
	public Entity
      {
      public:
	//
	// constructors & destructors
	//
	Remote();
	~Remote();

	//
	// attributes
	//
	State			state;
	elle::Timer		timer;
	elle::Channel*		channel;

	//
	// interfaces
	//

	// dumpable
	elle::Status	Dump(const elle::Natural32 = 0) const;
      };

      //
      // constructors & destructors
      //
      Agent();
      ~Agent();

      //
      // methods
      //
      elle::Status	Create(const elle::KeyPair&);
      elle::Status	Create(const elle::PublicKey&,
			       elle::Channel*);

      elle::Status	Authenticate();
      elle::Status	Destroy();

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
      Type		type;

      elle::PublicKey	K;

      union
      {
	Local*		local;
	Remote*		remote;
      };

      //
      // template methods
      //
      template <typename... T>
      elle::Status	Encrypt(T&...) const;
      elle::Status	Decrypt(const elle::Code&,
				elle::Clear&) const;
      elle::Status	Sign(const elle::Plain&,
			     elle::Signature&) const;
      template <typename... T>
      elle::Status	Verify(T&...) const;

      //
      // variadic templates
      //

      // decrypt
      template <typename T,
		typename... TT>
      elle::Status	Decrypt(const elle::Code&,
				T&,
				TT&...) const;

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
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/user/Agent.hxx>

#endif
