#include <cstdlib>
#include <fstream>
#include <sstream>
#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

#include <boost/algorithm/string.hpp>

#include <openssl/sha.h>

#include <QString>
#include <QByteArray>
#include <QLocalSocket>
#include <QProcess>

#include <elle/format/json.hh>
#include <elle/log.hh>
#include <elle/network/Host.hh>
#include <elle/os/path.hh>
#include <elle/Passport.hh>
#include <elle/serialize/HexadecimalArchive.hh>
#include "elle/utility/Time.hh"

#include <common/common.hh>

#include <lune/Dictionary.hh>
#include <lune/Identity.hh>
#include <lune/Lune.hh>

#include <nucleus/neutron/Permissions.hh>

#include <elle/idiom/Close.hh>

#include "State.hh"

#include <signal.h>

ELLE_LOG_COMPONENT("infinit.surface.gap");

#define _REGISTER_CALLBACK_HANDLER(data, indice, handler)               \
  template<>                                                            \
  void                                                                  \
  State::attach_callback<data>(std::function<void (data const*)> callback) \
  {                                                                     \
    if(_notification_handler.find(indice) != _notification_handler.end()) \
      {                                                                 \
        return;                                                         \
      }                                                                 \
  _notification_handler.insert(                                         \
    std::make_pair(                                                     \
      indice,                                                           \
      new handler(callback)));                                          \
  }                                                                     \
/**/

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
    {
    }

    // - State ----------------------------------------------------------------

    State::State()
      : _meta{
      new plasma::meta::Client{
        common::meta::host(),
          common::meta::port(),
          true,
        }}
      , _trophonius{
        new plasma::trophonius::Client{
          common::trophonius::host(),
          common::trophonius::port(),
          true,
        }}
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
          this->_meta->token(str);
          std::getline(identity_file, str);
          this->_meta->identity(str);
          std::getline(identity_file, str);
          this->_meta->email(str);
        }
    }

    State::~State()
    {
      for (auto& it: this->_networks)
        delete (it).second;
      this->_networks.clear();

      delete this->_meta;
      this->_meta = nullptr;

      delete this->_trophonius;
      this->_trophonius = nullptr;
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
      if (!conn.waitForConnected(2000))
        throw Exception{
            gap_internal_error,
              "Couldn't connect to the watchdog:" + conn.error()
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
      ELLE_DEBUG("Command sent");

      if (response == nullptr)
        return;

      if (!conn.waitForReadyRead(2000))
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

    User const& State::user(std::string const& id)
    {
      auto it = this->_users.find(id);
      if (it != this->_users.end())
        return *(it->second);

      auto response = this->_meta->user(id);
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
      auto response = this->_meta->user_from_public_key(public_key);
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
      auto res = this->_meta->search_users(text);
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
      this->_meta->token("");
      auto res = this->_meta->login(email, password);

      ELLE_DEBUG("Logged in as %s token = %s", email, res.token);

      this->_me._id = res._id;
      this->_me.fullname = res.fullname;
      this->_me.email = res.email;
      this->_me.public_key = "";

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
          if (identity.Restore(identity_clear)  == elle::Status::Error)
            throw Exception(gap_internal_error,
                            "Cannot save the identity file.");

          identity.store(email);

          // user.dic
          lune::Dictionary dictionary;

          dictionary.store(email);
        }
    }

    void
    State::logout()
    {
      this->_meta->logout();
    }

    void
    State::ask_notif(int i)
    {
      json::Dictionary request;
      request["recipient_id"] = this->_me._id;
      request["notification_id"] = i;

      switch(i)
        {
          case 0:
            request["debug"] = "U mad ?";
            break;
          case 1:
            request["sender_id"] = this->_me._id;
            request["transaction_id"] = 875456789;
            request["file_name"] = "[Childporn] Fabien 28/10/1992.png";
            request["file_size"] = 8294624;
            break;
          case 2:
            request["transaction_id"] = 875456789;
            request["status"] = 1;
            break;
          default:
            break;
        }

      this->_meta->debug_ask_notif(request);
    }

    std::string
    State::invite_user(std::string const& email)
    {
      auto response = this->_meta->invite_user(email);

      return response._id;
    }

    void
    State::send_file_to_new_user(std::string const& recipient_email,
                                 std::string const& file_path)
    {
      std::string recipient_id = this->invite_user(recipient_email);

      this->send_file(recipient_id,
                      file_path);
    }

    void
    State::send_file(std::string const& recipient_id,
                     std::string const& file_path)
    {
      if (!fs::exists(file_path))
        return;

      int size = get_size(file_path);
      std::string name = fs::path(file_path).filename().string();

      // Build timestamp.
      elle::utility::Time time;
      time.Current();

      // Create an ostream to convert timestamp to string.
      std::ostringstream oss;
      oss << time.nanoseconds;

      // FIXME: How to compute network name ?
      std::string network_name =
        this->_me._id +
        + " - "
        + recipient_id
        + " - "
        + name
        + " - "
        + oss.str();

      ELLE_DEBUG("Creating temporary network '%s'.", network_name);

      // std::string network_id = this->create_network(network_name);

      // this->refresh_networks();

      // this->network_add_user(network_id, recipient_id);

      // this->_meta->send_file(recipient_id,
      //                        name,
      //                        size,
      //                        fs::is_directory(file_path));

    }

    void
    State::send_message(std::string const& recipient_id,
                        std::string const& message)
    {
      this->_meta->send_message(recipient_id,
                                this->_me._id,
                                message);
    }


    void
    State::register_(std::string const& fullname,
                     std::string const& email,
                     std::string const& password,
                     std::string const& activation_code)
    {
      // Logout first, and ignore errors.
      try { this->logout(); } catch (elle::HTTPException const&) {}

      this->_meta->register_(email, fullname, password, activation_code);
      ELLE_DEBUG("Registered new user %s <%s>", fullname, email);
      this->login(email, password);
    }

    _REGISTER_CALLBACK_HANDLER(gap_UserStatusNotification,
                               8,
                               plasma::trophonius::Client::UserStatusHandler)

    _REGISTER_CALLBACK_HANDLER(gap_FileTransferRequestNotification,
                               7,
                               plasma::trophonius::Client::FileTransferRequestHandler)

    _REGISTER_CALLBACK_HANDLER(gap_FileTransferStatusNotification,
                               11,
                               plasma::trophonius::Client::FileTransferStatusHandler)

    _REGISTER_CALLBACK_HANDLER(gap_MessageNotification,
                               217,
                               plasma::trophonius::Client::MessageHandler)

    _REGISTER_CALLBACK_HANDLER(gap_BiteNotification,
                               0,
                               plasma::trophonius::Client::BiteHandler)


    template<typename T>
    void
    State::attach_callback(std::function<void(T const*)> callback)
    {
      static_assert(sizeof(T) == 0, "Call of attach_callback should be specialized.");
      return;
    }

    bool
    State::poll()
    {
      std::unique_ptr<json::Dictionary> dic = this->_trophonius->poll();

      if(!dic)
        return false;

      if(!dic->contains("notification_id"))
        return false;

      int notification_id = (*dic)["notification_id"].as_integer();

      handlerMap::const_iterator handler =
        _notification_handler.find(notification_id);

      if (handler == _notification_handler.end())
        return false;

      (*handler).second->call(dic.get());

      dic.release();

      return true;
    }

    bool
    State::has_device() const
    {
      return fs::exists(common::passport_path());
    }

    void
    State::update_device(std::string const& name, bool force_create)
    {
      ELLE_DEBUG("Registering device %s", name);

      std::string passport_string;

      if (force_create || !this->has_device())
        {
          auto res = this->_meta->create_device(name);
          passport_string = res.passport;
          ELLE_DEBUG("Created device id: %s", res.created_device_id);
        }
      else
        {
          elle::Passport passport;

          passport.load(elle::io::Path(lune::Lune::Passport));

          ELLE_DEBUG("Passport id: %s", passport.id);
          auto res = this->_meta->update_device(passport.id, name);
          passport_string = res.passport;
        }

      elle::Passport passport;
      if (passport.Restore(passport_string) == elle::Status::Error)
        throw Exception(gap_internal_error, "Cannot load the passport");

      passport.store(elle::io::Path(lune::Lune::Passport));
    }

    //- Network management ----------------------------------------------------

    std::string
    State::create_network(std::string const& name)
    {
      auto response = this->_meta->create_network(name);
      this->_networks_dirty = true;
      this->_networks_status_dirty = true;
      return response.created_network_id;
    }

    std::map<std::string, Network*> const&
    State::networks()
    {
      if (this->_networks_dirty)
        {
          auto response = this->_meta->networks();
          for (auto const& network_id: response.networks)
            {
              if (this->_networks.find(network_id) == this->_networks.end())
                {
                  auto response = this->_meta->network(network_id);
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
      auto it = this->networks().find(network_id);
      assert(it != this->networks().end());
      Network* network = it->second;
      assert(network != nullptr);

      std::string const& group_binary = common::infinit::binary_path("8group");

      QStringList arguments;
      arguments << "--user" << _meta->email().c_str()
                << "--type" << "user"
                << "--add"
                << "--network" << network->_id.c_str()
                << "--identity" << this->user(user_id).public_key.c_str()
                ;
      ELLE_DEBUG("LAUNCH: %s %s",
                      group_binary,
                      arguments.join(" ").toStdString());
      QProcess p;
      p.start(group_binary.c_str(), arguments);
      if (!p.waitForFinished())
        throw Exception(gap_internal_error, "8group binary failed");
      if (p.exitCode())
        throw Exception(gap_internal_error, "8group binary exited with errors");
      auto res = this->_meta->network_add_user(network_id, user_id);
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
      if (!fs::exists(common::passport_path()))
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

      std::string watchdog_binary = common::infinit::binary_path("8watchdog");

      ELLE_WARN("Launching binary: %s", watchdog_binary);
      QProcess p;
      if (p.execute(watchdog_binary.c_str()) < 0)
        throw Exception(gap_internal_error, "Cannot start the watchdog !");

      // Connect to the new watchdog instance
      QLocalSocket conn;
      int tries = 0;
      while (tries++ < 5)
        {
          conn.connectToServer(common::watchdog::server_name().c_str());
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
          this->_send_watchdog_cmd("run", &args);
        }
    }

    /// - File level ----------------------------------------------------------

    FileInfos const&
    State::file_infos(std::string const& path)
    {
      std::string abspath = elle::os::path::absolute(path, true);
      ELLE_DEBUG("Get file infos of %s", abspath);
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
      arguments << "--user" << this->_meta->email().c_str()
                << "--type" << "user"
                << "--network" << this->network(infos->network_id)._id.c_str()
                << "--path" << ("/" + infos->relative_path).c_str()
                << "--consult"
                ;

      QProcess p;
      ELLE_DEBUG("LAUNCH: %s %s", access_binary, arguments.join(" ").toStdString());
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
          if (permissions & nucleus::neutron::permissions::read)
            gap_perm |= gap_read;
          if (permissions & nucleus::neutron::permissions::write)
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
      arguments << "--user" << _meta->email().c_str()
                << "--type" << "user"
                << "--grant"
                << "--network" << network->_id.c_str()
                << "--path" << ("/" + infos.relative_path).c_str()
                << "--identifier" << this->user(user_id).public_key.c_str()
                ;
      if (permissions & gap_read)
        arguments << "--read";
      if (permissions & gap_write)
        arguments << "--write";

      ELLE_DEBUG("LAUNCH: %s %s", access_binary, arguments.join(" ").toStdString());

      if (permissions & gap_exec)
        {
          ELLE_WARN("XXX: setting executable permissions not yet implemented");
        }

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

    size_t
    State::get_size(fs::path const& path)
    {
      if (!fs::exists(path))
        return 0;

      if (!fs::is_directory(path))
        return fs::file_size(path);

      size_t size = 0;
      fs::directory_iterator end_itr;

      for (fs::directory_iterator itr(path);
           itr != end_itr;
           ++itr)
        {
          if (fs::is_directory(itr->status()))
            {
              size += get_size(itr->path());
            }
          else
            {
              size += fs::file_size(itr->path());
            }
        }
      return size;
    }

    // - TROPHONIUS ----------------------------------------------------
    /// Connect to trophonius
    void
    State::connect()
    {
      this->_trophonius->connect(this->_me._id,
                                 this->_meta->token());

      ELLE_DEBUG("Connect to trophonius with 'id': %s and 'token':  %s",
                 this->_meta->identity(), this->_meta->token());
    }
  }
}
