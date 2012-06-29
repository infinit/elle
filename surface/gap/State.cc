
#include <cstdlib>
#include <fstream>
#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

#include <boost/filesystem.hpp>

#include <QString>
#include <QByteArray>
#include <QLocalSocket>

#include <elle/log.hh>
#include <elle/network/Host.hh>

#include <lune/Dictionary.hh>
#include <lune/Identity.hh>

#include <elle/idiom/Close.hh>

#include <plasma/common/resources.hh>

#include "API.hh"
#include "State.hh"

namespace surface
{
  namespace gap
  {
    namespace fs = boost::filesystem;

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

    namespace json = elle::format::json;

    struct Network
    {
      std::string name;
    };

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
            throw std::runtime_error("Cannot get current user");
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
      throw std::runtime_error("Cannot find any network for path '" + path_string + "'");
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
        throw std::runtime_error("Watchdog id is too long!");
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
              throw std::runtime_error("Couldn't send the command '" + cmd + "'");
        }
      else
        throw std::runtime_error("Couldn't connect to the watchdog");
    }

    // XXX no hash occurs
    std::string State::_hash_password(std::string const& email,
                                      std::string const& password)
    {
      return password;
    }

    void State::login(std::string const& email, std::string const& password)
    {
      auto res = this->_api->login(email, password);

      if (!res.success)
        throw std::runtime_error(res.error);

      elle::log::debug("Logged in as", email, "token =", res.token);

      std::string identity_clear;

      lune::Identity      identity;

      // Decrypt the identity
        {
          if (identity.Restore(res.identity)    == elle::Status::Error ||
              identity.Decrypt(password)        == elle::Status::Error ||
              identity.Clear()                  == elle::Status::Error ||
              identity.Save(identity_clear)     == elle::Status::Error
             )
            throw("Couldn't decrypt the identity file !");
        }

      // Store the identity
        {
          // user.idy
          if (identity.Restore(identity_clear)  == elle::Status::Error ||
              identity.Store(email)             == elle::Status::Error)
            throw std::runtime_error("Cannot save the identity file.");

          // user.dic
          lune::Dictionary dictionary;
          if (dictionary.Store(email) == elle::Status::Error)
            throw std::runtime_error("Cannot store the dictionary.");
        }
    }

    void State::register_(std::string const& fullname,
                          std::string const& email,
                          std::string const& password)
    {
      auto res = this->_api->register_(email, fullname, password);
      if (!res.success)
          throw std::runtime_error(res.error);
      elle::log::debug("Registered new user", fullname, email);
      this->login(email, password);
    }

    namespace detail
    {
        std::string get_local_address()
        {
          elle::network::Host::Container hosts;

          if (elle::network::Host::Hosts(hosts) == elle::Status::Error)
            throw std::runtime_error("Couldn't retreive host list");

          if (!hosts.size())
            throw std::runtime_error("No usable host found !");

          std::string host;
          hosts[0].Convert(host);
          return host;
        }
    }

    void State::update_device(std::string const& name, short port)
    {
      std::string local_address = detail::get_local_address();
      elle::log::debug("Registering new device", name, "for host:", local_address);

      fs::path passport_path(_infinit_home);
      passport_path /=  "infinit.ppt";
      if (!fs::exists(passport_path))
        {
          //auto res = this->_api->sync_CreateDevice(name, local_address, port);
        }
      else
        {

        }
    }

  }
}

