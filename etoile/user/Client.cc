//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Client.cc
//
// created       julien quintard   [thu mar 11 16:21:11 2010]
// updated       julien quintard   [thu mar 11 17:14:30 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Client.hh>

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
    Client::Client():
      agent(NULL)
    {
    }

    ///
    /// destructor,
    ///
    Client::~Client()
    {
      Client::Scoutor	scoutor;

      // release the agent.
      if (this->agent != NULL)
	delete this->agent;

      // go through the applications and delete them.
      for (scoutor = this->applications.begin();
	   scoutor != this->applications.end();
	   scoutor++)
	delete *scoutor;
    }

  }
}
