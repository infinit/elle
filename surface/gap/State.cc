
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

#include <plasma/common/resources.hh>

#include "API.hh"
#include "State.hh"

namespace surface
{
  namespace gap
  {

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

    State::State(std::string const& updater_path)
      : _infinit_home(getenv("INFINIT_HOME"))
      , _networks()
      , _networks_dirty(true)
      , _api(new API)
      , _updater_path(updater_path)
    {
      if (this->_infinit_home.empty())
        {
          struct passwd *pw = ::getpwuid(::getuid());
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
    }

    void State::update_infinit_home()
    {
      if (::system(this->_updater_path.c_str()) != 0)
        throw std::runtime_error("Couldn't launch the process");
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
      json::Dictionary req;
      req["email"] = email;
      req["password"] = this->_hash_password(email, password);
      auto res = this->_api->post("/user/login", req);
      if ((*res)["success"] == false)
        throw std::runtime_error((*res)["error"].as<std::string>());
    }

    void State::register_(std::string const& fullname,
                          std::string const& email,
                          std::string const& password)
    {
      json::Dictionary req;
      req["fullname"] = fullname;
      req["email"] = email;
      req["password"] = password;
      req["admin_token"] = "fdjskfdakl;asdklwqioefwiopfdsjkl;daskl;askl;fsd";

      auto res = this->_api->post("/user/register", req);
      auto& dict = dynamic_cast<json::Dictionary&>(*res);
      elle::log::debug("Got register response:", res->repr());
    }

  }
}

