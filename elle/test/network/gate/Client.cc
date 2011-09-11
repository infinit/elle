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
// updated       julien quintard   [fri sep  2 20:42:02 2011]
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

      // create the locus.
      if (this->locus.Create(line) == StatusError)
	escape("unable to create the locus");

      leave();
    }

    ///
    /// this method is the thread entry locus.
    ///
    Status		Client::Run()
    {
      enter();

      std::cout << "[locus]" << std::endl;
      this->locus.Dump();

      // register the message.
      if (Network::Register(
	    Procedure<TagChallenge>(
	      Callback<>::Infer(&Client::Challenge, this))) == StatusError)
	escape("unable to register the challenge message");

      // create the gate.
      if (this->gate.Create() == StatusError)
	escape("unable to create the gate");

      // connect the gate.
      if (this->gate.Connect(this->locus) == StatusError)
	escape("unable to connect to the bridge");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles messages.
    ///
      Status		Client::Challenge(const String&		text)
    {
      String		response("RESPONSE");

      enter();

      // simply display the text.
      std::cout << "[Challenge] " << text << std::endl;

      // reply.
      if (this->gate.Reply(
	    Inputs<TagResponse>(response)) == StatusError)
	escape("unable to return the response");

      leave();
    }

  }
}
