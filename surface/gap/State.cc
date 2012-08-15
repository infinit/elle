
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <openssl/sha.h>

#include <QString>
#include <QByteArray>
#include <QLocalSocket>
#include <QProcess>

#include <elle/format/json.hh>
#include <elle/log.hh>
#include <elle/network/Host.hh>
#include <elle/os/path.hh>
#include <elle/serialize/HexadecimalArchive.hh>

#include <common/common.hh>

#include <lune/Dictionary.hh>
#include <lune/Identity.hh>
#include <lune/Passport.hh>

#include <nucleus/neutron/Permissions.hh>

#include <elle/idiom/Close.hh>

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

    // - State ----------------------------------------------------------------

    State::State()
      : log{elle::log::Logger::Level::warn, "gap"}
      , _api{
          new plasma::meta::Client{
              common::meta::host(),
              common::meta::port(),
              true,
              log,
          }
      }
      , _users{}
      , _files_infos{}
      , _networks{}
      , _networks_dirty{true}
      , _networks_status{}
      , _networks_status_dirty{true}
    {
      std::ifstream identity_file{common::watchdog::identity_path()};

      if (identity_file.good())
        {
          std::string str;
          std::getline(identity_file, str);
          this->_api->token(str);
          std::getline(identity_file, str);
          this->_api->identity(str);
          std::getline(identity_file, str);
          this->_api->email(str);
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
      this->_networks_status_dirty = true;
    }

    void State::_reload_networks()
    {
    }

    std::string State::_watchdog_id() const // XXX should be cached
    {
      boost::filesystem::path wtg_id_path(common::infinit::home());
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
      conn.connectToServer(common::watchdog::server_name().c_str());
      if (conn.waitForConnected(2000))
        {
          json::Dictionary req;
          req["_id"] = this->_watchdog_id();
          req["command"] = cmd;
          if (kwargs != nullptr)
            req.update(*kwargs);
          this->log.debug("Send watchdog command:", req.repr());
          conn.write(req.repr().c_str());
          conn.write("\n");
          if (!conn.waitForBytesWritten(2000))
              throw Exception(gap_internal_error,
                              "Couldn't send the command '" + cmd + "'");
          this->log.debug("Command sent");

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
              auto ptr = json::parse(ss);
              response->update(ptr->as_dictionary());
            }
        }
      else
        throw Exception(gap_internal_error, "Couldn't connect to the watchdog");
    }

    User const& State::user(std::string const& id)
    {
      auto it = this->_users.find(id);
      if (it != this->_users.end())
        return *(it->second);

      auto response = this->_api->user(id);
      std::unique_ptr<User> user{new User{
          response._id,
          response.fullname,
          response.email,
          response.public_key,
      }};

      this->_users[response._id] = user.get();
      return *(user.release());
    }

    User const&
    State::user_from_public_key(std::string const& public_key)
    {
      for (auto const& pair : this->_users)
        {
          if (pair.second->public_key == public_key)
            return *(pair.second);
        }
      auto response = this->_api->user_from_public_key(public_key);
      std::unique_ptr<User> user{new User{
          response._id,
          response.fullname,
          response.email,
          response.public_key,
      }};

      this->_users[response._id] = user.get();
      return *(user.release());
    }

    std::map<std::string, User const*>
    State::search_users(std::string const& text)
    {
      std::map<std::string, User const*> result;
      auto res = this->_api->search_users(text);
      for (auto const& user_id : res.users)
        {
          result[user_id] = &this->user(user_id);
        }
      return result;
    }

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
      this->_api->token("");
      auto res = this->_api->login(email, password);

      this->log.debug("Logged in as", email, "token =", res.token);

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

    void
    State::logout()
    {
      this->_api->logout();
    }

    void
    State::register_(std::string const& fullname,
                     std::string const& email,
                     std::string const& password,
                     std::string const& activation_code)
    {
      // Logout first, and ignore errors.
      try { this->logout(); } catch (plasma::meta::Exception const&) {}

      this->_api->register_(email, fullname, password, activation_code);
      this->log.debug("Registered new user", fullname, email);
      this->login(email, password);
    }

    namespace detail
    {
        std::string
        get_local_address()
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

    bool
    State::has_device() const
    {
      return fs::exists(common::passport_path());
    }

    void
    State::update_device(std::string const& name, bool force_create)
    {
      std::string local_address = detail::get_local_address();
      this->log.debug("Registering new device", name, "for host:", local_address);

      std::string passport_string;

      if (force_create || !this->has_device())
        {
          auto res = this->_api->create_device(name, local_address, 0);
          passport_string = res.passport;
          this->log.debug("Created device id:", res.created_device_id);
        }
      else
        {
          lune::Passport passport;

          if (passport.Load() == elle::Status::Error)
            throw Exception(gap_internal_error, "Cannot load the passport");

          this->log.debug("Passport id:", passport.id);
          auto res = this->_api->update_device(
              passport.id,
              &name,
              &local_address,
              0);

          passport_string = res.passport;
        }

      lune::Passport passport;
      if (passport.Restore(passport_string) == elle::Status::Error)
        throw Exception(gap_internal_error, "Cannot load the passport");
      if (passport.Store() == elle::Status::Error)
        throw Exception(gap_internal_error, "Cannot save the passport");
    }

    //- Network management ----------------------------------------------------

    void
    State::create_network(std::string const& name)
    {
      auto response = this->_api->create_network(name);
      this->_networks_dirty = true;
      this->_networks_status_dirty = true;
    }

    std::map<std::string, Network*> const&
    State::networks()
    {
      if (this->_networks_dirty)
        {
          auto response = this->_api->networks();
          for (auto const& network_id: response.networks)
            {
              if (this->_networks.find(network_id) == this->_networks.end())
                {
                  auto response = this->_api->network(network_id);
                  this->_networks[network_id] = new Network{response};
                }
            }
          this->_networks_dirty = false;
        }
      return this->_networks;
    }

    Network const&
    State::network(std::string const& id)
    {
      auto it = this->networks().find(id);
      if (it == this->networks().end())
        throw Exception{
            gap_error,
            "Cannot find any network for id '" + id + "'"
        };
      return *(it->second);
    }

    void
    State::network_add_user(std::string const& network_id,
                            std::string const& user)
    {
      // makes user we have an id
      std::string user_id = this->user(user)._id;
      auto res = this->_api->network_add_user(network_id, user_id);
      auto it = this->networks().find(network_id);
      assert(it != this->networks().end());
      Network* network = it->second;
      assert(network != nullptr);
      if (std::find(network->users.begin(),
                    network->users.end(),
                    user_id) == network->users.end())
        network->users.push_back(user_id);
    }

    std::map<std::string, NetworkStatus*> const&
    State::networks_status()
    {
      if (this->_networks_status_dirty)
        {
          json::Dictionary response;
          this->_send_watchdog_cmd("status", nullptr, &response);

          auto& networks = response["networks"].as_array();
          for (size_t i = 0; i < networks.size(); ++i)
            {
              auto& network = networks[i].as_dictionary();
              std::string mount_point = network["mount_point"].as_string();
              std::string network_id = network["_id"].as_string();

              NetworkStatus* status = nullptr;
              auto it = this->_networks_status.find(network_id);
              if (it == this->_networks_status.end())
                {
                  status = this->_networks_status[network_id]
                         = new NetworkStatus;
                }
              else
                {
                  status = it->second;
                }
              assert(status != nullptr);
              *status = NetworkStatus{
                  network_id,
                  mount_point,
              };
            }
          this->_networks_status_dirty = false;
        }
      return this->_networks_status;
    }

    NetworkStatus const&
    State::network_status(std::string const& id)
    {
      auto it = this->networks_status().find(id);
      if (it == this->networks_status().end())
        throw Exception{
            gap_error,
            "Unknown network id '" + id + "'",
        };
      return *(it->second);
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
              conn.connectToServer(common::watchdog::server_name().c_str());
              if (!conn.waitForConnected(2000))
                break;
              conn.disconnectFromServer();
            }
          this->log.debug("Waiting", sleep_time,
                           "secs for the old watchdog to be stopped (", tries, " / 10 )");
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
      if (!fs::exists(common::passport_path()))
        throw Exception(gap_no_device_error,
                        "Cannot start infinit without any local device");

      if (this->_api->token().size() == 0 ||
          this->_api->identity().size() == 0 ||
          this->_api->email().size() == 0)
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
          this->log.warn("Couldn't stop the watchdog:", err.what());
        }

      std::string watchdog_binary = common::infinit::binary_path("8watchdog");

      this->log.info("Launching binary:", watchdog_binary);
      QProcess p;
      if (p.execute(watchdog_binary.c_str()) < 0)
        throw Exception(gap_internal_error, "Cannot start the watchdog !");

      // Connect to the new watchdog instance
      QLocalSocket conn;
      int tries = 0;
      while (tries++ < 5)
        {
          conn.connectToServer(common::watchdog::server_name().c_str());
          this->log.debug("Trying to connect to the new watchdog");
          if (conn.waitForConnected(2000))
            break;
          this->log.debug("Retrying to connect (", tries, ")");
          ::sleep(1);
        }
      if (!conn.isValid())
        throw Exception(gap_internal_error, "Couldn't connect to the new watchdog instance");

      this->log.debug("Connected to the watchdog");

      // Getting the new watchdog id
      // When connected, the watchdog id file should exists
      std::string new_watchdog_id;
      tries = 0;
      do {
          if (tries > 0) ::sleep(1);
          try { new_watchdog_id = this->_watchdog_id(); }
          catch (std::exception const& err)
            {
              this->log.warn("Cannot read the new watchdog id:", err.what());
            }
          if (new_watchdog_id.size() && old_watchdog_id != new_watchdog_id)
            {
              this->log.debug("Found new watchdog id:", new_watchdog_id);
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
          args["user"] = this->_api->email();
          this->_send_watchdog_cmd("run", &args);
        }
    }

    /// - File level ----------------------------------------------------------

    FileInfos const&
    State::file_infos(std::string const& path)
    {
      std::string abspath = elle::os::path::absolute(path, true);
      this->log.debug("Get file infos of", abspath);
      auto it = this->_files_infos.find(abspath);
      if (it != this->_files_infos.end())
        return *(it->second);

      std::unique_ptr<FileInfos> infos;

      for (auto const& pair : this->networks_status())
        {
          std::string mount_point = pair.second->mount_point;

          if (boost::algorithm::starts_with(abspath, mount_point))
            {
              std::string relpath;
              if (abspath == mount_point)
                relpath = "";
              else
                relpath = abspath.substr(mount_point.size() + 1);

              infos.reset(new FileInfos{
                  mount_point,
                  pair.first,
                  abspath,
                  relpath,
                  {},
              });
              break;
            }
        }

      if (infos.get() == nullptr)
        throw Exception(gap_error,
                        "Cannot find network for path '" + abspath + "'");

      std::string const& access_binary = common::infinit::binary_path("8access");

      QStringList arguments;
      arguments << "--user" << this->_api->email().c_str()
                << "--type" << "user"
                << "--network" << this->network(infos->network_id).name.c_str()
                << "--path" << ("/" + infos->relative_path).c_str()
                << "--consult"
                ;

      QProcess p;
      this->log.debug("LAUNCH:", access_binary, arguments.join(" ").toStdString());
      p.start(access_binary.c_str(), arguments);
      if (!p.waitForFinished()) // .8 sec
        throw Exception(gap_internal_error, "8access binary failed");
      std::stringstream ss{p.readAllStandardOutput().constData()};

      std::string line;
      while(std::getline(ss, line))
        {
          if (!line.size())
            continue;
          std::stringstream line_stream{line};
          std::string public_key;
          int permissions;
          line_stream >> public_key /* eat "User" */
                      >> public_key
                      >> permissions;
          int gap_perm = (int) gap_none;
          if (permissions & nucleus::neutron::PermissionRead)
            gap_perm |= gap_read;
          if (permissions & nucleus::neutron::PermissionWrite)
            gap_perm |= gap_write;
          std::string const& user_id = this->user_from_public_key(public_key)._id;
          infos->accesses[user_id] = gap_perm;
        }

      this->_files_infos[abspath] = infos.get();
      return *(infos.release());
    }

    void State::set_permissions(std::string const& user_id,
                                std::string const& abspath,
                                int permissions,
                                bool recursive)
    {
      FileInfos const& infos = this->file_infos(abspath);
      auto it = this->networks().find(infos.network_id);

      if (it == this->networks().end())
        throw Exception(gap_error, "Wrong network id");

      Network* network = it->second;

      std::string const& access_binary = common::infinit::binary_path("8access");

      QStringList arguments;
      arguments << "--user" << _api->email().c_str()
                << "--type" << "user"
                << "--grant"
                << "--network" << network->name.c_str()
                << "--path" << ("/" + infos.relative_path).c_str()
                << "--identifier" << this->user(user_id).public_key.c_str()
                ;
      if (permissions & gap_read)
        arguments << "--read";
      if (permissions & gap_write)
        arguments << "--write";

      this->log.debug("LAUNCH:", access_binary, arguments.join(" ").toStdString());

      if (permissions & gap_exec)
        this->log.warn(
          "XXX: setting executable permissions not yet implemented");

      QProcess p;
      p.start(access_binary.c_str(), arguments);
      if (!p.waitForFinished())
        throw Exception(gap_internal_error, "8access binary failed");

      if (recursive && elle::os::path::is_directory(abspath))
        {
          boost::filesystem::directory_iterator it{abspath},
                                                end{};
          for (; it != end; ++it)
            {
              this->set_permissions(user_id,
                                    it->path().string(),
                                    permissions,
                                    true);
            }
        }
    }

  }
}

