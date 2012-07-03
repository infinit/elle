#ifndef  SURFACE_GAP_STATE_HH
# define SURFACE_GAP_STATE_HH

# include <string>
# include <map>

# include <plasma/meta/Client.hh>

# include "gap.h"

namespace surface
{
  namespace gap
  {

    struct Network;

    class Exception
      : public std::runtime_error
    {
    public:
      gap_Status const code;

    public:
      Exception(gap_Status code, std::string const& msg);
    };

    class State
    {
    private:
      std::string                     _infinit_home;
      std::map<std::string, Network*> _networks;
      bool                            _networks_dirty;
      plasma::meta::Client*           _api;

    public:
      State();
      ~State();
      void refresh_networks();
      std::string const& path_to_network(std::string const& path);
      void login(std::string const& email, std::string const& password);
      void register_(std::string const& fullname,
                     std::string const& email,
                     std::string const& password);
      void update_device(std::string const& name,
                         bool force_create = false);
      std::string hash_password(std::string const& email,
                                std::string const& password);

    /// properties
    public:
      std::map<std::string, Network*> const& networks() const;

    private:
      std::string _watchdog_id() const;
      void _send_watchdog_cmd(std::string const& cmd);
      void _reload_networks();
    };

  }
}


#endif

