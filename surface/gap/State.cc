#include "State.hh"
#include "_detail/Process.hh"
#include "MetricReporter.hh"

#include <common/common.hh>

#include <protocol/Serializer.hh>
#include <protocol/ChanneledStream.hh>

#include <etoile/portal/Portal.hh>

#include <reactor/network/tcp-socket.hh>
#include <reactor/sleep.hh>

#include <elle/log.hh>
#include <elle/log/TextLogger.hh>
#include <elle/os/path.hh>
#include <elle/os/getenv.hh>

// XXX[WTF?]
//#include <elle/serialize/HexadecimalArchive.hh>
//#include <elle/utility/Time.hh>
//#include <elle/io/Path.hh>
//#include <elle/io/Piece.hh>
//#include <elle/HttpClient.hh>
//#include <elle/CrashReporter.hh>
//
//
//
//#include <lune/Dictionary.hh>
//#include <lune/Identity.hh>
//#include <lune/Lune.hh>
//#include <lune/Phrase.hh>
//
//#include <nucleus/neutron/Permissions.hh>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <fstream>

ELLE_LOG_COMPONENT("infinit.surface.gap.State");

namespace surface
{
  namespace gap
  {
    namespace fs = boost::filesystem;

    // - Exception ------------------------------------------------------------
    Exception::Exception(gap_Status code, std::string const& msg)
      : std::runtime_error(msg)
      , code(code)
    {}

    // - State ----------------------------------------------------------------
    State::State()
      : _meta{new plasma::meta::Client{
          common::meta::host(), common::meta::port(), true,
        }}
      , _trophonius{nullptr}
      , _users{}
      , _logged{false}
      , _swaggers_dirty{true}
      , _output_dir{common::system::download_directory()}
      , _files_infos{}
      , _networks{}
      , _networks_dirty{true}
      , _infinit_instance_manager{}
    {
      std::string log_file = elle::os::getenv("INFINIT_LOG_FILE", "");
      if (log_file.size() > 0)
        this->output_log_file(log_file);

      ELLE_LOG("Creating a new State");

      this->transaction_callback(
          [&] (TransactionNotification const &n, bool is_new) -> void {
            this->_on_transaction(n, is_new);
          }
      );
      this->transaction_status_callback(
          [&] (TransactionStatusNotification const &n, bool) -> void {
            this->_on_transaction_status(n);
          }
      );
      this->user_status_callback(
          [&] (UserStatusNotification const &n) -> void {
            this->_on_user_status_update(n);
          }
      );

      std::string user = elle::os::getenv("INFINIT_USER", "");

      if (user.length() > 0)
      {
        std::string identity_path = common::infinit::identity_path(user);

        if (identity_path.length() > 0 && fs::exists(identity_path))
        {
          std::ifstream identity;
          identity.open(identity_path);

          if (!identity.good())
            return;

          std::string token;
          std::getline(identity, token);

          std::string ident;
          std::getline(identity, ident);

          std::string mail;
          std::getline(identity, mail);

          std::string id;
          std::getline(identity, id);

          this->_meta->token(token);
          this->_meta->identity(ident);
          this->_meta->email(mail);

          this->_me = this->_meta->self();
        }
      }
      // Initialize server.
      metrics::google::server(common::metrics::google_server(),
                              common::metrics::google_port(),
                              "cd",
                              metrics::google::retrieve_id(
                                common::metrics::id_path()));
    }

    void
    State::output_log_file(std::string const& path)
    {
      static std::ofstream out{
          path,
          std::fstream::app | std::fstream::out
      };
      elle::log::logger(
          std::unique_ptr<elle::log::Logger>{new elle::log::TextLogger(out)}
      );
    }

    State::State(std::string const& token):
      State{}
    {
      ELLE_LOG("Creating a new State with token");
      this->_meta->token(token);
      auto res = this->_meta->self();
      this->_meta->identity(res.identity);
      this->_meta->email(res.email);
      //XXX factorize that shit
      this->_me = res;
    }


    State::~State()
    {
      ELLE_WARN("Destroying state.");
      this->logout();
    }

    void
    State::debug()
    {
      this->_meta->debug();
    }

    // - TROPHONIUS ----------------------------------------------------
    /// Connect to trophonius

    void
    State::_notify_8infinit(Transaction const& trans, reactor::Scheduler& sched)
    {
      ELLE_TRACE("Notify 8infinit for transaction %s", trans);

      namespace proto = infinit::protocol;
      std::string const& network_id = trans.network_id;

      /// Check if network is valid
      {
          auto network = this->networks().find(network_id);

          if (network == this->networks().end())
              throw gap::Exception{gap_internal_error, "Unable to find network"};

      }

      // Fetch Nodes and find the correct one to contact
      std::list<std::string> externals;
      std::list<std::string> locals;
      {
        std::string target_device{};

        if (trans.recipient_device_id == this->device_id())
          target_device = trans.sender_device_id;
        else
          target_device = trans.recipient_device_id;

        Endpoint e = this->_meta->device_endpoints(network_id,
                                                   this->device_id(),
                                                   target_device);

        externals = std::move(e.externals);
        locals = std::move(e.locals);
      }

      // Finish by calling the RPC to notify 8infinit of all the IPs of the peer
      {
        lune::Phrase phrase;

        if (this->_wait_portal(network_id) == false)
          throw Exception{gap_error, "Couldn't find portal to infinit instance"};

        phrase.load(this->_me._id, network_id, "portal");

        ELLE_DEBUG("Connect to the local 8infint instance (%s:%d)",
                   elle::String{"127.0.0.1"},
                   phrase.port);

        // Connect to the server.
        reactor::network::TCPSocket socket{
            sched,
            elle::String("127.0.0.1"),
            phrase.port,
        };

        proto::Serializer serializer{
            sched,
            socket
        };

        proto::ChanneledStream channels{
            sched,
            serializer
        };

        etoile::portal::RPC rpcs{channels};

        bool ok = false;
        auto send_to_slug = [&] (std::string const& endpoint) -> void {
          if (!ok)
          {
            std::vector<std::string> result;

            boost::split(result, endpoint, boost::is_any_of(":"));
            ELLE_DEBUG("slug_connect(%s, %s)", result[0], result[1]);

            rpcs.slug_connect(result[0], std::stoi(result[1]));

            if (!rpcs.slug_wait(result[0], std::stoi(result[1])))
              throw elle::Exception("unable to connect or authenticate both "
                                    "nodes.");
            ok = true;
          }
          else
              ELLE_DEBUG("ignore endpoint %s", endpoint);
        };

        try
          {
            ELLE_DEBUG("found endpoints: %s locals, %s externals",
                       locals.size(), externals.size());
            std::for_each(std::begin(externals), std::end(externals), send_to_slug);
            std::for_each(std::begin(locals), std::end(locals), send_to_slug);
          }
        catch (elle::Exception const& e)
          {
            ELLE_TRACE("caught exception %s", e.what());
            throw e;
          }
      }
    }

  }
}
