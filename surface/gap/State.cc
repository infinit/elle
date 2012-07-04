
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

#include <boost/filesystem.hpp>

#include <openssl/sha.h>

#include <QString>
#include <QByteArray>
#include <QLocalSocket>
#include <QProcess>

#include <elle/log.hh>
#include <elle/network/Host.hh>
#include <elle/serialize/HexadecimalArchive.hh>

#include <lune/Dictionary.hh>
#include <lune/Identity.hh>
#include <lune/Passport.hh>

#include <elle/idiom/Close.hh>

#include <plasma/common/resources.hh>

#include "API.hh"
#include "State.hh"

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

    // - Utilities ------------------------------------------------------------

    namespace
    {
      std::string getenv(char const* var)
      {
        char const* res = ::getenv(var);
        if (res == nullptr)
          return "";
        return res;
      }
    }

    // - Network --------------------------------------------------------------

    struct Network
    {
      std::string name;
    };

    // - State ----------------------------------------------------------------

    State::State()
      : _infinit_home(getenv("INFINIT_HOME"))
      , _networks()
      , _networks_dirty(true)
      , _api(new plasma::meta::Client("127.0.0.1", 12345))
    {
      if (this->_infinit_home.empty())
        {
          struct passwd *pw = ::getpwuid(::getuid());
          if (pw == nullptr || pw->pw_dir == nullptr)
            throw Exception(gap_internal_error, "Cannot get current user");
          boost::filesystem::path homedir(pw->pw_dir);
          homedir /= ".config/infinit";
          this->_infinit_home = homedir.string();
        }
    }

    State::~State()
    {
      for (auto& it: this->_networks)
        delete (it).second;
      this->_networks.clear();
      delete this->_api;
      this->_api = nullptr;
    }

    void State::refresh_networks()
    {
      this->_send_watchdog_cmd("refresh_networks");
      this->_networks_dirty = true;
    }

    std::string const& State::path_to_network(std::string const& path_string)
    {
      if (this->_networks_dirty)
        this->_reload_networks();
      throw Exception(gap_error, "Cannot find any network for path '" + path_string + "'");
    }

    void State::_reload_networks()
    {
    }

    std::string State::_watchdog_id() const // XXX should be cached
    {
      boost::filesystem::path wtg_id_path(this->_infinit_home);
      wtg_id_path /= "infinit.wtg";
      std::ifstream file(wtg_id_path.string());
      if (!file.good())
        throw std::runtime_error("Cannot open '" + wtg_id_path.string() + "'");

      char wtg_id[4096];
      file.read(wtg_id, 4096);
      if (!file.eof())
        throw Exception(gap_internal_error, "Watchdog id is too long!");
      return std::string(wtg_id, file.gcount());
    }

    void State::_send_watchdog_cmd(std::string const& cmd)
    {
      QString id = this->_watchdog_id().c_str();
      QLocalSocket conn;
      conn.connectToServer(WATCHDOG_SERVER_NAME);
      if (conn.waitForConnected(2000))
        {
          QByteArray json_cmd = QString("{"
                "\"command\":\"" + QString(cmd.c_str()) + "\","
                "\"_id\": \"" + id + "\""
          "}\n").toAscii();
          conn.write(json_cmd);
          if (!conn.waitForBytesWritten(2000))
              Exception(gap_internal_error, "Couldn't send the command '" + cmd + "'");
        }
      else
        throw Exception(gap_internal_error, "Couldn't connect to the watchdog");
    }

    // XXX no hash occurs
    std::string State::hash_password(std::string const& email,
                                     std::string const& password)
    {
      unsigned char hash[SHA256_DIGEST_LENGTH];
      SHA256_CTX context;
      std::string to_hash = email + "MEGABIET" + password + email + "MEGABIET";

      if (SHA256_Init(&context) == 0 ||
          SHA256_Update(&context, to_hash.c_str(), to_hash.size()) == 0 ||
          SHA256_Final(hash, &context) == 0)
        throw Exception(gap_internal_error, "Cannot hash login/password");

      std::ostringstream out;
      elle::serialize::OutputHexadecimalArchive ar(out);

      ar.SaveBinary(hash, SHA256_DIGEST_LENGTH);

      return out.str();
    }

    void State::login(std::string const& email, std::string const& password)
    {
      auto res = this->_api->login(email, password);

      if (!res.success)
        throw Exception(gap_api_error, res.error);

      elle::log::debug("Logged in as", email, "token =", res.token);

      std::string identity_clear;

      lune::Identity      identity;

      // Decrypt the identity
        {
          if (identity.Restore(res.identity)    == elle::Status::Error ||
              identity.Decrypt(password)        == elle::Status::Error ||
              identity.Clear()                  == elle::Status::Error ||
              identity.Save(identity_clear)     == elle::Status::Error)
            throw Exception(gap_internal_error,
                            "Couldn't decrypt the identity file !");
        }

      // Store the identity
        {
          // user.idy
          if (identity.Restore(identity_clear)  == elle::Status::Error ||
              identity.Store(email)             == elle::Status::Error)
            throw Exception(gap_internal_error,
                            "Cannot save the identity file.");

          // user.dic
          lune::Dictionary dictionary;
          if (dictionary.Store(email) == elle::Status::Error)
            throw Exception(gap_internal_error,
                            "Cannot store the dictionary.");
        }
    }

    void State::register_(std::string const& fullname,
                          std::string const& email,
                          std::string const& password)
    {
        {
          auto res = this->_api->register_(email, fullname, password);
          if (!res.success)
              throw Exception(gap_api_error, res.error);
        }
      elle::log::debug("Registered new user", fullname, email);
      this->login(email, password);
    }

    namespace detail
    {
        std::string get_local_address()
        {
          elle::network::Host::Container hosts;

          if (elle::network::Host::Hosts(hosts) == elle::Status::Error)
            throw Exception(gap_network_error, "Couldn't retrieve host list");

          if (!hosts.size())
            throw Exception(gap_network_error, "No usable host found !");

          std::string host;
          hosts[0].Convert(host);
          return host;
        }
    }

    void State::update_device(std::string const& name, bool force_create)
    {
      std::string local_address = detail::get_local_address();
      elle::log::debug("Registering new device", name, "for host:", local_address);

      fs::path passport_path(_infinit_home);
      passport_path /=  "infinit.ppt";

      std::string passport_string;

      if (force_create || !fs::exists(passport_path))
        {
          auto res = this->_api->create_device(name, local_address, 0);
          if (!res.success)
            throw Exception(gap_api_error, res.error);
          passport_string = res.passport;
          elle::log::debug("Created device id:", res.created_device_id);
        }
      else
        {
          lune::Passport passport;

          if (passport.Load() == elle::Status::Error)
            throw Exception(gap_internal_error, "Cannot load the passport");

          elle::log::debug("Passport id:", passport.id);
          auto res = this->_api->update_device(
              passport.id,
              &name,
              &local_address,
              0);

          if (!res.success)
            throw Exception(gap_api_error, res.error);
          passport_string = res.passport;
        }

      lune::Passport passport;
      if (passport.Restore(passport_string) == elle::Status::Error)
        throw Exception(gap_internal_error, "Cannot load the passport");
      if (passport.Store() == elle::Status::Error)
        throw Exception(gap_internal_error, "Cannot save the passport");
    }


    void State::create_network(std::string const& name)
    {
      this->_api->create_network(name);
    }

    std::map<std::string, Network*> const& State::networks()
    {
      if (this->_networks_dirty)
        {
          auto res = this->_api->networks();
          for (auto const& network_id: res.networks)
            {
              this->_networks[network_id] = nullptr; //XXX
            }
        }
      return this->_networks;
    }

    void State::stop_watchdog()
    {
      QString id(_watchdog_id().c_str());
      if (!id.size())
        return;

      // Stop old watchdog
      {
        QLocalSocket conn;
        conn.connectToServer(WATCHDOG_SERVER_NAME);
        if (conn.waitForConnected(2000))
          {
            QByteArray cmd = QString("{"
                  "\"command\":\"stop\","
                  "\"_id\": \"" + id + "\""
            "}\n").toAscii();
            conn.write(cmd);
            if (!conn.waitForBytesWritten(2000))
                throw std::runtime_error("Couldn't stop the old watchdog instance");
          }
        else
          {
            elle::log::warn("Couldn't connect to the old watchdog instance");
            return;
          }
      }

      // Waiting for the old server to be stopped
      {
        int tries = 1;
        do {
            QLocalSocket conn;
            conn.connectToServer(WATCHDOG_SERVER_NAME);
            if (!conn.waitForConnected(2000))
              break;
            elle::log::debug("Waiting for the old watchdog to be stopped (", tries, ')');
            ::sleep(1);
        } while (++tries < 10);

        if (tries >= 10)
          elle::log::warn("The old watchdog instance does not stop !");
      }
    }

    void State::launch_watchdog()
    {
      std::string old_watchdog_id;
      try
        {
          old_watchdog_id = this->_watchdog_id();
          this->stop_watchdog();
        }
      catch (std::exception const& err)
        {
          elle::log::warn("Couldn't stop the watchdog:", err.what());
        }

      fs::path watchdog_binary(_infinit_home);
      watchdog_binary /= "bin/8watchdog";
      elle::log::info("Launching binary:", watchdog_binary.string());
      QProcess p;
      if (p.execute(watchdog_binary.string().c_str()) < 0)
        throw Exception(gap_internal_error, "Cannot start the watchdog !");

      // Connect to the new watchdog instance
      QLocalSocket conn;
      int tries = 0;
      while (tries++ < 5)
        {
          conn.connectToServer(WATCHDOG_SERVER_NAME);
          elle::log::debug("Trying to connect to the new watchdog");
          if (conn.waitForConnected(2000))
            break;
          elle::log::debug("Retrying to connect (", tries, ")");
          ::sleep(1);
        }
      if (!conn.isValid())
        throw Exception(gap_internal_error, "Couldn't connect to the new watchdog instance");

      elle::log::debug("Connected to the watchdog");

      // Getting the new watchdog id
      // When connected, the watchdog id file should exists
      std::string new_watchdog_id;
      tries = 0;
      do {
          if (tries > 0) ::sleep(1);
          try { new_watchdog_id = this->_watchdog_id(); }
          catch (std::exception const&) {}
          if (new_watchdog_id.size() && old_watchdog_id != new_watchdog_id)
            {
              elle::log::debug("Found new watchdog id:", new_watchdog_id);
              break;
            }
      } while (++tries < 10);

      if (tries == 10)
        throw Exception(gap_internal_error, "Couldn't open infinit watchdog id file");

      // calling watchdog run command (which gives the meta token)
      QString token(this->_api->token().c_str());
      //QString identity(this->_identityUpdater.identity().c_str());
      //QByteArray cmd = QString("{"
      //    "\"command\":"  "\"run\""                       ","
      //    "\"_id\":"      "\"" + newWatchdogId + "\""     ","
      //    "\"token\":"    "\"" + token + "\""             ","
      //    "\"identity\":" "\"" + identity + "\""
      //    "}\n").toAscii();
      //conn.write(cmd);
      //if (!conn.waitForBytesWritten(2000))
      //  throw std::runtime_error("Couldn't run the watchdog");
    }

  }
}

