#ifndef  SURFACE_GAP_STATE_HH
# define SURFACE_GAP_STATE_HH

# include <string>
# include <map>

# include <plasma/metaclient/MetaClient.hh>

namespace surface
{
  namespace gap
  {

    struct Network;

    namespace meta = ::plasma::metaclient;

    class State
    {
    private:
      std::string                     _infinit_home;
      std::map<std::string, Network*> _networks;
      bool                            _networks_dirty;
      meta::MetaClient*               _api;

    public:
      State();
      ~State();
      void refresh_networks();
      std::string const& path_to_network(std::string const& path);
      void login(std::string const& email, std::string const& password);
      void register_(std::string const& fullname,
                     std::string const& email,
                     std::string const& password);
      void create_device(std::string const& name);

    private:
      std::string _watchdog_id() const;
      void _send_watchdog_cmd(std::string const& cmd);
      void _reload_networks();
      std::string _hash_password(std::string const& email,
                                 std::string const& password);
    };

  }
}


#endif

