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
      std::map<std::string, Network*> _networks;
    public:
      ~State();
      void refresh_networks();
      std::string const& path_to_network(std::string const& path) const;
    };

  }
}


#endif

