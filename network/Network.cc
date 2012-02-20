//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed feb  3 16:49:46 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Network.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the list of registered procedures.
    ///
    Network::Container                  Network::Procedures;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the network components.
    ///
    Status              Network::Initialize()
    {
      // initialize the local server.
      if (LocalServer::Initialize() == StatusError)
        escape("unable to initialize the local server");

      // initialize the TCP server.
      if (TCPServer::Initialize() == StatusError)
        escape("unable to initialize the TCP server");

      // initialize the session.
      if (Session::Initialize() == StatusError)
        escape("unable to initialize the session");

      return elle::StatusOk;
    }

    ///
    /// this method cleans the network components.
    ///
    Status              Network::Clean()
    {
      // clean the session.
      if (Session::Clean() == StatusError)
        escape("unable to clean the session");

      // clean the TCP server.
      if (TCPServer::Clean() == StatusError)
        escape("unable to clean the TCP server");

      // clean the local server.
      if (LocalServer::Clean() == StatusError)
        escape("unable to clean the local server");

      //
      // clear the container.
      //
      {
        Network::Scoutor        scoutor;

        // go through the functionoids.
        for (scoutor = Network::Procedures.begin();
             scoutor != Network::Procedures.end();
             scoutor++)
          {
            // delete the functionoid.
            delete scoutor->second;
          }

        // clear the container.
        Network::Procedures.clear();
      }

      return elle::StatusOk;
    }

    ///
    /// this method takes a newly received packet and dispatch it.
    ///
    /// note that the input variables are not tracked for automatic
    /// deletion because the caller should already been tracking them.
    ///
    Status              Network::Dispatch(std::shared_ptr<Parcel>& parcel)
    {
      //
      // first, try to  wake up a waiting slot.
      //
      {
        // try to wake up a slot.
        if (Fiber::Awaken(parcel->header->event, parcel) == StatusTrue)
            return elle::StatusOk;
      }

      //
      // if no slot is waiting for this event, dispatch it right away.
      //
      {
        auto it = Network::Procedures.find(parcel->header->tag);

        // retrieve the procedure's functionoid associated to the header's tag.
        if (it == Network::Procedures.end())
          {
            // test if the message received is an error, if so, log it.
            if (parcel->header->tag == TagError)
              {
                Report  report;

                // extract the error message.
                if (report.Extract(*parcel->data) == StatusError)
                  escape("unable to extract the error message");

                // report the remote error.
                transpose(report);

                // log the error.
                log("an error message has been received with no registered "
                    "procedure");
              }

            return elle::StatusOk;
          }

        // assign the new session.
        if (Session::Assign(parcel->session) == StatusError)
          escape("unable to assign the session");

        // call the functionoid.
        if (it->second->Call(*parcel->data) == StatusError)
          escape("an error occured while processing the event");

        // clear the session.
        if (Session::Clear() == StatusError)
          escape("unable to flush the session");
      }

      return elle::StatusOk;
    }

    ///
    /// this method dumps the procedures.
    ///
    Status              Network::Show(const Natural32           margin)
    {
      String            alignment(margin, ' ');
      Network::Scoutor  scoutor;

      std::cout << alignment << "[Network]" << std::endl;

      // go through the functionoids.
      for (scoutor = Network::Procedures.begin();
           scoutor != Network::Procedures.end();
           scoutor++)
        {
          // dump the functionoid.
          if (scoutor->second->Dump(margin + 2) == StatusError)
            escape("unable to dump the functionoid");
        }

      return elle::StatusOk;
    }

  }
}
