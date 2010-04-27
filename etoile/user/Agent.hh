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
// updated       julien quintard   [mon apr 26 18:54:28 2010]
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
      public Meta,
      public Dumpable
    {
    public:
      //
      // constants
      //
      static const Natural32		Expiration;

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
	public Meta,
	public Dumpable
      {
      public:
	//
	// attributes
	//
	PrivateKey	k;

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;
      };

      ///
      /// the remote class contains everything for communicating
      /// with an external agent process.
      ///
      struct Remote:
	public Meta,
	public Dumpable
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
	State		state;
	Timer		timer;
	Channel*	channel;

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;
      };

      //
      // constructors & destructors
      //
      Agent();
      ~Agent();

      //
      // methods
      //
      Status		Create(const KeyPair&);
      Status		Create(const PublicKey&,
			       Channel*);

      Status		Authenticate();
      Status		Destroy();

      //
      // callbacks
      //
      Status		Timeout();
      Status		Error(const String&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Type		type;

      PublicKey		K;

      union
      {
	Local*		local;
	Remote*		remote;
      };

      //
      // template methods
      //
      template <typename... T>
      Status		Encrypt(T&...) const;
      Status		Decrypt(const Code&,
				Clear&) const;
      Status		Sign(const Plain&,
			     Signature&) const;
      template <typename... T>
      Status		Verify(T&...) const;

      //
      // variadic templates
      //

      // decrypt
      template <typename T,
		typename... TT>
      Status		Decrypt(const Code&,
				T&,
				TT&...) const;

      // sign
      template <typename T1>
      Status		Sign(const T1&,
			     Signature&) const;
      template <typename T1,
		typename T2>
      Status		Sign(const T1&,
			     const T2&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     const T9&,
			     Signature&) const;

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

      Status		Decrypt(const Code&		code,
				Archive&		archive) const
      {
	return (this->Decrypt(code, (Plain&)archive));
      }

      Status		Sign(const Archive&		archive,
			     Signature&			signature) const
      {
	return (this->Sign((Plain&)archive, signature));
      }
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/user/Agent.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Client.hh>
#include <etoile/user/User.hh>

#endif
