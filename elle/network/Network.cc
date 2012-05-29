#include <elle/standalone/ReportSerializer.hxx>
#include <elle/network/Network.hh>
#include <elle/concurrency/Event.hh>

#include <elle/idiom/Close.hh>
#include <elle/log.hh>
#include <elle/idiom/Open.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Network");

namespace elle
{
  namespace network
  {
    ///
    /// this container holds the list of registered procedures.
    ///
    Network::Container                  Network::Procedures;

    ///
    /// this method initializes the network components.
    ///
    Status              Network::Initialize()
    {
      // initialize the session.
      if (Session::Initialize() == Status::Error)
        escape("unable to initialize the session");

      return Status::Ok;
    }

    ///
    /// this method cleans the network components.
    ///
    Status              Network::Clean()
    {
      // clean the session.
      if (Session::Clean() == Status::Error)
        escape("unable to clean the session");

      Network::Procedures.clear();

      return Status::Ok;
    }

    ///
    /// this method dumps the procedures.
    ///
    Status              Network::Show(const Natural32           margin)
    {
      String            alignment(margin, ' ');
      Network::Scoutor  scoutor;

      std::cout << alignment << "[Network]" << std::endl;

      return StatusOk;
    }

    Status
    Network::Register(Tag i, const Function& f)
    {
      ELLE_LOG_TRACE("register procedure for tag %s.", i);
      assert(f);
      std::pair<Network::Iterator, Boolean>     result;

      // check if this tag has already been recorded.
      if (Network::Procedures.find(i) != Network::Procedures.end())
        escape("this tag seems to have already been recorded");
      result = Network::Procedures.insert
        (std::pair<const Tag, Function>(i, f));

      // check if the insertion was successful.
      if (result.second == false)
        escape("unable to insert the selectoinoid in the container");
      // FIXME: duplicate, remove
      assert(Network::Procedures.find(i)->second);

      return StatusOk;
    }
  }
}
