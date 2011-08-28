//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infi...hole/implementations/cirkle/Neighbour.cc
//
// created       julien quintard   [wed aug 24 13:12:46 2011]
// updated       julien quintard   [sun aug 28 13:02:17 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/cirkle/Neighbour.hh>
#include <hole/implementations/cirkle/Cirkle.hh>
#include <hole/implementations/cirkle/Manifest.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Neighbour::Neighbour():
	state(StateUnauthenticated),
	gate(NULL)
      {
      }

      ///
      /// XXX
      ///
      Neighbour::~Neighbour()
      {
	// delete the gate, if present.
	if (this->gate != NULL)
	  delete this->gate;
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status	Neighbour::Create(const elle::Point&	point)
      {
	enter();

	// set the point.
	this->point = point;

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Neighbour::Create(elle::Gate*		gate)
      {
	elle::Callback<
	  elle::Status,
	  elle::Parameters<>
	  >				monitor(&Neighbour::Monitor, this);

	enter();

	// set the gate.
	this->gate = gate;

	// register the monitor callback.
	if (this->gate->Monitor(monitor) == elle::StatusError)
	  escape("unable to monitor the connection");

	//
	// in this case, we do not know the point the neiighbour is listening
	// on for incoming connections.
	//
	// therefore, this information must be requested.
	//

	// request the listening point.
	if (this->gate->Send(
	      elle::Inputs<TagListen>()) == elle::StatusError)
	  escape("unable to request the listening point");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Neighbour::Connect()
      {
	elle::Callback<
	  elle::Status,
	  elle::Parameters<>
	  >				monitor(&Neighbour::Monitor, this);

	enter();

	// allocate a gate.
	this->gate = new elle::Gate;

	// register the monitor callback.
	if (this->gate->Monitor(monitor) == elle::StatusError)
	  escape("unable to monitor the connection");

	// create the gate.
	if (this->gate->Create() == elle::StatusError)
	  escape("unable to create the gate");

	// connect the gate.
	if (this->gate->Connect(this->point) == elle::StatusError)
	  escape("unable to connect to the peer");

	leave();
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback is triggered whenever the socket state changes.
      ///
      elle::Status	Neighbour::Monitor()
      {
	enter();

	// depending on the state of the socket.
	switch (this->gate->state)
	  {
	  case elle::Channel::StateConnected:
	    {
	      // challenge the peer.
	      if (this->gate->Send(
		    elle::Inputs<TagChallenge>()) == elle::StatusError)
		escape("unable to send the challenge");

	      break;
	    }
	  case elle::Channel::StateDisconnected:
	    {
	      // if the neighbour has been registered in the routing table...
	      if (Cirkle::Machine.table.Exist(this->label) == elle::StatusTrue)
		{
		  // remove it.
		  if (Cirkle::Machine.table.Remove(this->label) ==
		      elle::StatusTrue)
		    escape("unable to remove the neighbour from the "
			   "routing table");
		}

	      // remove the neighbour from the neighbourhood.
	      if (Cirkle::Machine.neighbourhood.Remove(
		    this->point) == elle::StatusError)
		escape("unable to remove the neighbour from the "
		       "neighbourhood");

	      // bury the instance.
	      bury(this);

	      break;
	    }
	  default:
	    {
	      escape("unexpected socket state '%u'",
		     this->gate->state);
	    }
	  }

	leave();
      }

//
// ---------- object ----------------------------------------------------------
//

      ///
      /// this macro-function call generates the object.
      ///
      embed(Neighbour, _());

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this function dumps a neighbour object.
      ///
      elle::Status	Neighbour::Dump(elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	// display the name.
	std::cout << alignment << "[Neighbour] "
		  << std::hex << this << std::endl;

	// display the state.
	std::cout << alignment << elle::Dumpable::Shift << "[State] "
		  << this->state << std::endl;

	// dump the point.
	if (this->point.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the point");

	// dump the label.
	if (this->label.Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the label");

	// dump the gate, if present.
	if (this->gate != NULL)
	  {
	    if (this->gate->Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the gate");
	  }
	else
	  {
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[Gate] " << elle::none << std::endl;
	  }

	leave();
      }

//
// ---------- archivable ------------------------------------------------------
//

      ///
      /// this method serializes the neighbour object.
      ///
      elle::Status	Neighbour::Serialize(elle::Archive&	archive) const
      {
	enter();

	// serialize the neighbour's attributes.
	//
	// note that the live attributes such as the label and the
	// socket are not included.
	if (archive.Serialize(this->point) == elle::StatusError)
	  escape("unable to serialize the attributes");

	// XXX serialize the other stuff? label at least

	leave();
      }

      ///
      /// this method extracts the neighbour object.
      ///
      elle::Status	Neighbour::Extract(elle::Archive&	archive)
      {
	enter();

	// extract the attributes.
	if (archive.Extract(this->point) == elle::StatusError)
	  escape("unable to extract the attributes");

	// XXX extract the other stuff? label at least

	leave();
      }

    }
  }
}
