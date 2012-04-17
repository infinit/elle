//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 24 13:12:46 2011]
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
// ---------- definitions -----------------------------------------------------
//

      ///
      /// XXX 30 sec
      ///
      const elle::Natural32             Neighbour::Timeout = 30000;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Neighbour::Neighbour():
        state(StateUnauthenticated),
        port(0),
        gate(NULL),
        timer(NULL)
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

        // delete the timer, if present.
        if (this->timer != NULL)
          delete this->timer;
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status      Neighbour::Create(const elle::network::Locus&    locus)
      {
        // set the locus.
        this->locus = locus;

        // allocate the timer.
        this->timer = new elle::concurrency::Timer;

        // create the timer.
        if (this->timer->Create(elle::concurrency::Timer::ModeSingle) == elle::Status::Error)
          escape("unable to create the timer");

        // XXX -discard +abort

        // subscribe to the timer's signal.
        if (this->timer->signal.timeout.Subscribe(
              elle::Callback<>::Infer(&Neighbour::Discard,
                                      this)) == elle::Status::Error)
          escape("unable to subscribe to the signal");

        // start the timer.
        if (this->timer->Start(Neighbour::Timeout) == elle::Status::Error)
          escape("unable to start the timer");

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbour::Create(elle::Gate*           gate)
      {
        elle::Callback<
          elle::Status::,
          elle::Parameters<>
          >             monitor(&Neighbour::Monitor, this);

        // set the gate.
        this->gate = gate;

        // register the monitor callback.
        if (this->gate->Monitor(monitor) == elle::Status::Error)
          escape("unable to monitor the connection");

        // XXX add timer for timeout

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbour::Connect()
      {
        elle::Callback<
          elle::Status::,
          elle::Parameters<>
          >                             monitor(&Neighbour::Monitor, this);

        // allocate a gate.
        this->gate = new elle::Gate;

        // register the monitor callback.
        if (this->gate->Monitor(monitor) == elle::Status::Error)
          escape("unable to monitor the connection");

        // create the gate.
        if (this->gate->Create() == elle::Status::Error)
          escape("unable to create the gate");

        // connect the gate.
        if (this->gate->Connect(this->locus) == elle::Status::Error)
          escape("unable to connect to the peer");

        return elle::Status::Ok;
      }

//
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status      Neighbour::Discard()
      {
        // if the neighbour has been authenticated...
        if (this->state == Neighbour::StateAuthenticated)
          {
            // delete the timer.
            delete this->timer;

            // reset the locus.
            this->timer = NULL;
          }
        else
          {
            // remove the neighbour from the neighbourhood.
            if (Cirkle::Computer->neighbourhood.Remove(
                  this->locus) == elle::Status::Error)
              escape("unable to remove the neighbour from the "
                     "neighbourhood");

            // delete the neighbour.
            delete this; // XXX rely on signals instead
          }

        return elle::Status::Ok;
      }

      ///
      /// this callback is triggered whenever the socket state changes.
      ///
      elle::Status      Neighbour::Monitor()
      {
        // depending on the state of the socket.
        switch (this->gate->state)
          {
          case elle::Channel::StateConnected:
            {
              // challenge the peer.
              if (this->gate->Send(
                    elle::Inputs<TagChallenge>()) == elle::Status::Error)
                escape("unable to send the challenge");

              break;
            }
          case elle::Channel::StateDisconnected:
            {
              // if the neighbour has been registered in the routing table...
              if (Cirkle::Computer->routingtable.Exist(
                    this->label) == elle::Status::True)
                {
                  // remove it.
                  if (Cirkle::Computer->routingtable.Remove(
                        this->label) == elle::Status::Error)
                    escape("unable to remove the neighbour from the "
                           "routing table");
                }

              // remove the neighbour from the neighbourhood.
              if (Cirkle::Computer->neighbourhood.Remove(
                    this->locus) == elle::Status::Error)
                escape("unable to remove the neighbour from the "
                       "neighbourhood");

              // delete the neighbour.
              delete this; // XXX rely on signals instead

              break;
            }
          default:
            {
              escape("unexpected socket state '%u'",
                     this->gate->state);
            }
          }

        return elle::Status::Ok;
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this function dumps a neighbour object.
      ///
      elle::Status      Neighbour::Dump(elle::Natural32 margin) const
      {
        elle::String    alignment(margin, ' ');

        // display the name.
        std::cout << alignment << "[Neighbour] "
                  << std::hex << this << std::endl;

        // display the state.
        std::cout << alignment << elle::Dumpable::Shift << "[State] "
                  << this->state << std::endl;

        // dump the locus.
        if (this->locus.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the locus");

        // dump the label.
        if (this->label.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the label");

        // dump the port.
        std::cout << alignment << elle::Dumpable::Shift
                  << "[Port] " << std::dec << this->port << std::endl;

        // dump the gate, if present.
        if (this->gate != NULL)
          {
            if (this->gate->Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the gate");
          }
        else
          {
            std::cout << alignment << elle::Dumpable::Shift
                      << "[Gate] " << elle::none << std::endl;
          }

        return elle::Status::Ok;
      }

    }
  }
}
