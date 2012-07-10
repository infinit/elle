
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

#include <common/common.hh>

#include <elle/format/json.hh>
#include <elle/log.hh>
#include <elle/network/Host.hh>
#include <elle/serialize/HexadecimalArchive.hh>

#include <lune/Dictionary.hh>
#include <lune/Identity.hh>
#include <lune/Passport.hh>

#include <elle/idiom/Close.hh>

#include <plasma/common/resources.hh>

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

    // - State ----------------------------------------------------------------

    State::State()
      : _api(new plasma::meta::Client("127.0.0.1", 12345))
      , _files_infos()
      , _networks()
      , _networks_dirty(true)
    {
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

    void State::_reload_networks()
    {
    }

    std::string State::_watchdog_id() const // XXX should be cached
    {
      boost::filesystem::path wtg_id_path(common::infinit_home());
      wtg_id_path /= "infinit.wtg";
      std::ifstream file(wtg_id_path.string());
      if (!file.good())
        throw std::runtime_error("Cannot open '" + wtg_id_path.string() + "'");

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
      QLocalSocket conn;
      conn.connectToServer(WATCHDOG_SERVER_NAME);
      if (conn.waitForConnected(2000))
        {
          json::Dictionary req;
          req["_id"] = this->_watchdog_id();
          req["command"] = cmd;
          if (kwargs != nullptr)
            req.update(*kwargs);
          elle::log::debug("Send watchdog command:", req.repr());
          conn.write(req.repr().c_str());
          conn.write("\n");
          if (!conn.waitForBytesWritten(2000))
              throw Exception(gap_internal_error,
                              "Couldn't send the command '" + cmd + "'");
          elle::log::debug("Command sent");

          if (response != nullptr)
            {
              if (!conn.waitForReadyRead(2000))
                throw Exception(gap_internal_error,
                              "Couldn't read response of '" + cmd + "' command");
              QByteArray response_data = conn.readLine();
              std::stringstream ss{
                  std::string{
                      response_data.data(),
                      static_cast<size_t>(response_data.size()),
                  },
              };
              auto ptr = json::Parser<>{}.Parse(ss);
              response->update(dynamic_cast<json::Dictionary const&>(*ptr));
            }
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

      fs::path passport_path(common::infinit_home());
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
      this->_networks_dirty = true;
    }

    std::map<std::string, Network*> const& State::networks()
    {
      if (this->_networks_dirty)
        {
          auto res = this->_api->networks();
          for (auto const& network_id: res.networks)
            {
              if (this->_networks.find(network_id) == this->_networks.end())
                {
                  this->_networks[network_id] =
                    new Network{this->_api->network(network_id)};
                }
            }
          this->_networks_dirty = false;
        }
      return this->_networks;
    }

    void State::stop_watchdog()
    {
      this->_send_watchdog_cmd("stop");
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

    void State::launch_watchdog(std::string const& watchdog_path)
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

      std::string watchdog_binary;
      if (watchdog_path.size())
        watchdog_binary = watchdog_path;
      else
        watchdog_binary =
          fs::path(common::infinit_home()).append("bin/8watchdog", fs::path::codecvt()).string();

      elle::log::info("Launching binary:", watchdog_binary);
      QProcess p;
      if (p.execute(watchdog_binary.c_str()) < 0)
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
          catch (std::exception const& err)
            {
              elle::log::warn("Cannot read the new watchdog id:", err.what());
            }
          if (new_watchdog_id.size() && old_watchdog_id != new_watchdog_id)
            {
              elle::log::debug("Found new watchdog id:", new_watchdog_id);
              break;
            }
      } while (++tries < 10);

      if (tries == 10)
        throw Exception(gap_internal_error, "Couldn't open infinit watchdog id file");

      // calling watchdog run command (which gives the meta token)
        {
          json::Dictionary args;
          args["token"] = this->_api->token();
          args["identity"] = this->_api->identity();
          this->_send_watchdog_cmd("run", &args);
        }
    }

    /// - File level ----------------------------------------------------------

    FileInfos const& State::file_infos(std::string const& abspath)
    {
      auto it = this->_files_infos.find(abspath);
      if (it != this->_files_infos.end())
        return *(it->second);

      json::Dictionary request, response;
      request["absolute_path"] = abspath;
      this->_send_watchdog_cmd("file_infos", &request, &response);

      std::unique_ptr<FileInfos> infos{new FileInfos{
          response["mount_point"].as<std::string>(),
          response["network_id"].as<std::string>(),
          response["absolute_path"].as<std::string>(),
          response["relative_path"].as<std::string>(),
      }};

      this->_files_infos[abspath] = infos.get();
      return *(infos.release());
    }

    void State::set_permissions(std::string const& user_id,
                                std::string const& abspath,
                                int permissions)
    {
      FileInfos const& infos = this->file_infos(abspath);
      auto it = this->networks().find(infos.network_id);

      if (it == this->networks().end())
        throw Exception(gap_error, "Wrong network id");

      Network* network = it->second;

      fs::path access_path{common::infinit_home()};
      access_path /= "bin";
      access_path /= "8access";

      QStringList arguments;
      arguments << "--user" << _api->email().c_str()
                << "--type" << "user"
                << "--grant" << user_id.c_str()
                << "--network" << network->name.c_str()
                << "--path" << infos.relative_path.c_str();

      if (permissions & gap_read)
        arguments << "--read";
      if (permissions & gap_write)
        arguments << "--write";

      if (permissions & gap_exec)
        elle::log::warn(
          "XXX: setting executable permissions not yet implemented");

      QProcess p;
      if (p.execute(access_path.string().c_str(), arguments) < 0)
        throw Exception(gap_internal_error, "Cannot start the watchdog !");
    }



  }
}

