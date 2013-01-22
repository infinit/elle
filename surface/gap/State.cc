#include <cstdlib>
#include <fstream>
#include <sstream>
#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include <openssl/sha.h>

#include <QString>
#include <QByteArray>
#include <QLocalSocket>
#include <QProcess>

#include <elle/format/json.hh>
#include <elle/log.hh>
#include <elle/network/Host.hh>
#include <elle/os/path.hh>
#include <elle/os/getenv.hh>
#include <elle/Passport.hh>
#include <elle/serialize/HexadecimalArchive.hh>
#include <elle/utility/Time.hh>
#include <elle/io/Path.hh>
#include <elle/io/Piece.hh>
#include <elle/HttpClient.hh>
#include <elle/CrashReporter.hh>

#include <reactor/network/tcp-socket.hh>
#include <reactor/sleep.hh>
#include <protocol/Serializer.hh>
#include <protocol/ChanneledStream.hh>
#include <etoile/portal/Portal.hh>

#include <common/common.hh>

#include <lune/Dictionary.hh>
#include <lune/Identity.hh>
#include <lune/Lune.hh>
#include <lune/Phrase.hh>

#include <nucleus/neutron/Permissions.hh>

#include <elle/idiom/Close.hh>
#include <surface/gap/MetricReporter.hh>
#include "State.hh"

#include <boost/algorithm/string.hpp>

#include <signal.h>

#include "_detail/Process.hh"

ELLE_LOG_COMPONENT("infinit.surface.gap.State");

namespace surface
{
  namespace gap
  {
    namespace fs = boost::filesystem;
    namespace json = elle::format::json;

    // - Exception ------------------------------------------------------------
    Exception::Exception(gap_Status code, std::string const& msg)
      : std::runtime_error(msg)
      , code(code)
    {}

    // - State ----------------------------------------------------------------
    State::State()
      : _meta{new plasma::meta::Client{
        common::meta::host(),
          common::meta::port(),
          true,
          }
        }
      , _trophonius{nullptr}
      , _users{}
      , _logged{false}
      , _swaggers_dirty{true}
      , _output_dir{common::system::download_directory()}
      , _files_infos{}
      , _networks{}
      , _networks_dirty{true}
      , _networks_status{}
      , _networks_status_dirty{true}
    {
      // XXX degeu !
      ELLE_LOG("Creating a new State");

      namespace p = std::placeholders;
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

      /*
        rajouter on_swagger_online
        dans swagger_online -> foreach network ; if swagger in network then
        call RPC qui informe 8infinit
       */

      std::string user = elle::os::getenv("INFINIT_USER", "");

      if (user.length() > 0)
      {
        std::string identity_path = common::watchdog::identity_path(user);

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

          this->_me = static_cast<User const&>(this->_meta->self());
        }
      }
    }

    void
    State::output_log_file(std::string const& path)
    {
      static std::ofstream* out = new std::ofstream{
          path,
          std::fstream::app | std::fstream::out
      };
      char const* components[] = {
          "infinit.surface.gap",
          "infinit.surface.gap.State",
          "infinit.surface.gap.MetricReporter",
          "infinit.plasma.trophonius.Client",
          "infinit.plasma.meta.Client",
          "elle.HTTPClient",
      };

      for (auto component : components)
        {
          elle::log::logger(component).output(*out);
          elle::log::logger(component).level(
              elle::log::Logger::Level::debug
          );
        }
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
      this->_me = static_cast<User const&>(res);
    }


    State::~State()
    {
      ELLE_WARN("Destroying state.");
      for (auto& it: this->_networks)
        delete it.second;
      this->_networks.clear();
      this->logout();
    }

    void
    State::debug()
    {
      this->_meta->debug();
    }

    void State::refresh_networks()
    {
      this->_send_watchdog_cmd("refresh_networks");
      this->_networks_dirty = true;
      this->_networks_status_dirty = true;
    }

    std::string State::_watchdog_id() const // XXX should be cached
    {
      std::string watchdog_id_file = common::watchdog::id_path(_me._id);
      std::ifstream file(watchdog_id_file);
      if (!file.good())
        throw std::runtime_error("Cannot open '" + watchdog_id_file + "'");

      char wtg_id[4096];
      file.read(wtg_id, 4096);
      if (!file.eof())
        throw Exception(gap_internal_error, "Watchdog id is too long!");
      std::string id(wtg_id, file.gcount());
      file.close();
      return id;
    }

    void
    State::_send_watchdog_cmd(std::string const& cmd,
                              elle::format::json::Dictionary const* kwargs,
                              elle::format::json::Dictionary* response)
    {
      ELLE_TRACE("Send watchdog command");
      QLocalSocket conn;

      conn.connectToServer(common::watchdog::server_name(this->_me._id).c_str());
      if (!conn.waitForConnected(2000))
        throw Exception{
            gap_internal_error,
              "Couldn't connect to the watchdog:" + std::to_string(conn.error())
        };

      json::Dictionary req;
      req["_id"] = this->_watchdog_id();
      req["command"] = cmd;
      if (kwargs != nullptr)
        req.update(*kwargs);
      ELLE_DEBUG("Send watchdog command: %s", req.repr());
      conn.write(req.repr().c_str());
      conn.write("\n");
      if (!conn.waitForBytesWritten(2000))
          throw Exception(gap_internal_error,
                          "Couldn't send the command '" + cmd + "'");

      ELLE_DEBUG("Command '%s' sent.", cmd);

      if (response == nullptr)
        {
          ELLE_DEBUG("Watchdog response is ignored for call %s.", cmd);
          return;
        }

      ELLE_DEBUG("Waiting for response.");

      if (!conn.waitForReadyRead(1000)) // Infinit is maybe too long, or not.
        throw Exception{
            gap_internal_error,
            "Couldn't read response of '" + cmd + "' command"
        };

      QByteArray response_data = conn.readLine();
      std::stringstream ss{
          std::string{
              response_data.data(),
              static_cast<size_t>(response_data.size()),
          },
      };
      auto ptr = json::parse(ss);
      response->update(ptr->as_dictionary());
    }

    //- Watchdog --------------------------------------------------------------

    void
    State::stop_watchdog()
    {
      this->_send_watchdog_cmd("stop");
      // Waiting for the old server to be stopped
      int tries = 1;
      int sleep_time = 2;
      do {
            {
              QLocalSocket conn;
              conn.connectToServer(
                  common::watchdog::server_name(this->_me._id).c_str()
              );
              if (!conn.waitForConnected(2000))
                break;
              conn.disconnectFromServer();
            }
            ELLE_DEBUG("Waiting %s secs for the old watchdog to be stopped (%s / 10 )",
                       sleep_time, tries);
          ::sleep(sleep_time);
          sleep_time += 2;
      } while (++tries < 10);

      if (tries >= 10)
        throw Exception(gap_internal_error,
                        "The old watchdog instance does not stop !");
      this->_networks_status_dirty = true;
    }

    void
    State::launch_watchdog()
    {
      if (!this->has_device())
        throw Exception(gap_no_device_error,
                        "Cannot start infinit without any local device");

      if (this->_meta->token().size() == 0 ||
          this->_meta->identity().size() == 0 ||
          this->_meta->email().size() == 0)
        throw Exception(gap_not_logged_in,
                        "Cannot start infinit anonymously");

      std::string old_watchdog_id;
      try
        {
          old_watchdog_id = this->_watchdog_id();
          this->stop_watchdog();
        }
      catch (std::exception const& err)
        {
          ELLE_WARN("Couldn't stop the watchdog: %s", err.what());
        }

      ELLE_ASSERT(this->_me._id.size() != 0);

      elle::os::path::make_path(
          common::infinit::user_directory(this->_me._id)
      );
      std::string watchdog_binary = common::infinit::binary_path("8watchdog");

      QStringList arguments;
      arguments << this->_me._id.c_str();

      ELLE_DEBUG("Launching binary: %s with id: %s", watchdog_binary, this->_me._id);
      if (QProcess::execute(watchdog_binary.c_str(), arguments) < 0)
        throw Exception(gap_internal_error, "Cannot start the watchdog");

      // Connect to the new watchdog instance
      QLocalSocket conn;
      int tries = 0;
      while (tries++ < 10)
        {
          conn.connectToServer(common::watchdog::server_name(this->_me._id).c_str());
          ELLE_DEBUG("Trying to connect to the new watchdog");
          if (conn.waitForConnected(2000))
            break;
          ELLE_DEBUG("Retrying to connect (%s)", tries);
          ::sleep(1);
        }
      if (!conn.isValid())
        throw Exception(gap_internal_error, "Couldn't connect to the new watchdog instance");

      ELLE_DEBUG("Connected to the watchdog");

      // Getting the new watchdog id
      // When connected, the watchdog id file should exists
      std::string new_watchdog_id;
      tries = 0;
      do {
          if (tries > 0) ::sleep(1);
          try { new_watchdog_id = this->_watchdog_id(); }
          catch (std::exception const& err)
            {
              ELLE_WARN("Cannot read the new watchdog id: %s", err.what());
            }
          if (new_watchdog_id.size() && old_watchdog_id != new_watchdog_id)
            {
              ELLE_DEBUG("Found new watchdog id: %s", new_watchdog_id);
              break;
            }
      } while (++tries < 10);

      if (tries == 10)
        throw Exception(gap_internal_error, "Couldn't open infinit watchdog id file");

      // calling watchdog run command (which gives the meta token)
        {
          json::Dictionary args;
          args["token"] = this->_meta->token();
          args["identity"] = this->_meta->identity();
          args["user"] = this->_meta->email();
          args["user_id"] = this->_me._id;
          this->_send_watchdog_cmd("run", &args);
        }
    }

    /// - File level ----------------------------------------------------------



    // - TROPHONIUS ----------------------------------------------------
    /// Connect to trophonius

    void
    State::_notify_8infinit(Transaction const& trans)
    {
      ELLE_TRACE("Notify 8infinit for transaction %s", trans);

      namespace proto = infinit::protocol;
      std::string const& sender_device = trans.sender_device_id;
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
          Endpoint e = this->_meta->device_endpoints(network_id, sender_device);

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

        reactor::Sleep s(elle::concurrency::scheduler(),
                         boost::posix_time::seconds(10));

        s.run();
        // Connect to the server.
        reactor::network::TCPSocket socket{
            elle::concurrency::scheduler(),
            elle::String("127.0.0.1"),
            phrase.port,
        };

        proto::Serializer serializer{
            elle::concurrency::scheduler(),
            socket
        };

        proto::ChanneledStream channels{
            elle::concurrency::scheduler(),
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
            ok = true;
          }
          else
              ELLE_DEBUG("ignore %s", endpoint);
        };

        try
        {
            std::for_each(std::begin(locals), std::end(locals), send_to_slug);
            std::for_each(std::begin(externals), std::end(externals), send_to_slug);
        }
        catch (elle::Exception const &e)
        {
            ELLE_TRACE("Caught exception %s", e.what());
            throw e;
        }
      }
    }

  }
}
