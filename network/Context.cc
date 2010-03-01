//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Context.cc
//
// created       julien quintard   [thu feb 25 14:02:27 2010]
// updated       julien quintard   [sun feb 28 11:02:16 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Context.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the context instance is initially null.
    ///
    Context::Context*		Environment = NULL;

//
// ---------- constructors & destructors --------------------------------------
//

    // XXX
    

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the context.
    ///
    Status		Context::Initialize(Socket*		socket,
					    const Address&	address)
    {
      // if a context already exists, delete it.
      if (Context::Environment != NULL)
	delete Context::Environment;

      // then, create a new one.
      //Context::Environment = new Context(socket, address);

      leave();
    }

    ///
    /// this method returns the instance of the context.
    ///
    Context*		Context::Instance()
    {
      // XXX check NULL

      return (Context::Environment);
    }
  }
}
