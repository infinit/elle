//
// ---------- header ----------------------------------------------------------
//
// project       8network
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8network/Network.hh
//
// created       julien quintard   [sat mar 27 08:37:14 2010]
// updated       julien quintard   [sun apr 18 20:01:15 2010]
//

#ifndef NETWORK_NETWORK_HH
#define NETWORK_NETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
#include <elle/idiom/Open.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class overwrites the agent methods so that no network communication
  /// occurs since this application already has access to the user's key
  /// pair.
  ///
  class Agent:
    public etoile::user::Agent
  {
  public:
    //
    // constructors & destructors.
    //
    Agent(const KeyPair&					pair):
      pair(pair)
    {
    }

    //
    // methods
    //
    Status		Decrypt(const Code&			code,
				Clear&				clear) const
    {
      return (this->pair.k.Decrypt(code, clear));
    }

    Status		Sign(const Plain&			plain,
			     Signature&				signature)
      const
    {
      return (this->pair.k.Sign(plain, signature));
    }

    //
    // attributes
    //
    KeyPair		pair;
  };

  ///
  /// this class implements the 8network application.
  ///
  class Network
  {
  public:
    //
    // constants
    //
    static const String			Path;

    //
    // static methods
    //
    static Status	New(const String&);
  };

}

#endif
