#ifndef  SURFACE_GAP_STATE_HH
# define SURFACE_GAP_STATE_HH

# include <string>
# include <map>
namespace surface
{
  namespace gap
  {

    struct Network;
    class API;

    class State
    {
    private:
      std::string                     _infinit_home;
      std::map<std::string, Network*> _networks;
      bool                            _networks_dirty;
      API*                            _api;
      std::string                     _updater_path;

    public:
      State(std::string const& updater_path);
      ~State();
      void update_infinit_home();
      void refresh_networks();
      std::string const& path_to_network(std::string const& path);
      void login(std::string const& email, std::string const& password);
      void register_(std::string const& fullname,
                     std::string const& email,
                     std::string const& password);
    private:
      std::string _watchdog_id() const;
      void _send_watchdog_cmd(std::string const& cmd);
      void _reload_networks();
      std::string _hash_password(std::string const& email, std::string const& password);
    };

  }
}


#endif

