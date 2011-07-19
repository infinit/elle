//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/gate/Client.cc
//
// created       julien quintard   [sun feb  7 01:32:45 2010]
// updated       julien quintard   [tue jul 19 16:51:32 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/gate/Client.hh>

namespace elle
{
  namespace test
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the client.
    ///
    Status		Client::Setup(const String&		line)
    {
      enter();

      // create the address.
      if (this->address.Create(line) == StatusError)
	escape("unable to create the address");

      leave();
    }

    ///
    /// this method is the thread entry point.
    ///
    Status		Client::Run()
    {
      Callback< Status,
		Parameters<const String,
			   String> >		challenge(&Client::Challenge,
							  this);

      enter();

      std::cout << "[address]" << std::endl;
      this->address.Dump();

      // register the message.
      if (Network::Register(
	    Procedure<TagChallenge,
		      TagResponse>(challenge)) == StatusError)
	escape("unable to register the challenge message");

      // create the gate.
      if (this->gate.Create() == StatusError)
	escape("unable to create the gate");

      // connect the gate.
      if (this->gate.Connect(this->address) == StatusError)
	escape("unable to connect to the bridge");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles messages.
    ///
    Status		Client::Challenge(const String&		text,
					  String&		response)
    {
      enter();

      // simply display the text.
      std::cout << "[Challenge] " << text << std::endl;

      // assign the response.
      response.assign("RESPONSE");

      leave();
    }

  }
}
