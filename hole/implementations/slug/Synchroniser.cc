//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [mon dec  5 14:27:18 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/slug/Synchroniser.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status	Synchroniser::Start()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Synchroniser::Start()\n");

	// create the timer.
	if (this->timer.Create(
	      elle::Timer::ModeSingle) == elle::StatusError)
	  escape("unable to create the timer");

	// subscribe to the timer's signal.
	if (this->timer.signal.timeout.Subscribe(
	      elle::Callback<>::Infer(&Synchroniser::Run,
				      this)) == elle::StatusError)
	  escape("unable to subscribe to the signal");

	// start the timer.
	if (this->timer.Start() == elle::StatusError)
	  escape("unable to start the timer");

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status	Synchroniser::Run()
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::slug::Synchroniser::Run()\n");

	// XXX
	// pseudo code:
	//
	// 1) send a message to one of the peers in order to retrieve its
	//    bank i.e the list of block addresses he has.
	//    note: in order to optimise we could select the peer with the
	//      latest timestamp i.e which has been in the network the
	//      longest.
	// 2) we get the bank, and for every address:
	//   a) we ignore it if we have already;
	//   b) or we request it.
	//   c) and we Fiber::Yield() to be sure not to monopolize the
	//      execution time.
	// XXX

	// emit the signal.
	if (this->signal.synchronised.Emit() == elle::StatusError)
	  escape("unable to emit the signal");

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the synchroniser.
      ///
      elle::Status	Synchroniser::Dump(const elle::Natural32 margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Synchroniser]" << std::endl;

	// dump the timer.
	if (this->timer.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the timer");

	leave();
      }

    }
  }
}
