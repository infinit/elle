#ifndef  SURFACE_GAP_STATE_HH
# define SURFACE_GAP_STATE_HH

# include <string>
# include <map>

namespace surface
{
  namespace gap
  {

    struct Network;

    class State
    {
    private:
      std::string                     _infinit_home;
      std::map<std::string, Network*> _networks;
      bool                            _networks_dirty;
    public:
      State();
      ~State();
      void refresh_networks();
      std::string const& path_to_network(std::string const& path);
    private:
      std::string _watchdog_id() const;
      void _send_watchdog_cmd(std::string const& cmd);
      void _reload_networks();
    };

  }
}


#endif

