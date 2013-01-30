#ifndef  SURFACE_GAP_INFINITINSTANCEMANAGER_HH
# define SURFACE_GAP_INFINITINSTANCEMANAGER_HH

# include <elle/system/Process.hh>

# include <map>
# include <memory>
# include <string>

namespace surface
{
  namespace gap
  {

    typedef std::unique_ptr<elle::system::Process> SystemProcessPtr;

    struct InfinitInstance
    {
      std::string       network_id;
      std::string       mount_point;
      SystemProcessPtr  process;
    };

    class InfinitInstanceManager
    {
    private:
      std::string _user_id;

    private:
      typedef std::unique_ptr<InfinitInstance> InfinitInstancePtr;
      std::map<std::string, InfinitInstancePtr> _instances;

    public:
      explicit
      InfinitInstanceManager(std::string const& user_id);

      void
      launch_network(std::string const& network_id);

      bool
      has_network(std::string const& network_id) const;

      void
      stop_network(std::string const& network_id);

      InfinitInstance const&
      network_instance(std::string const& network_id) const;

      InfinitInstance const*
      network_instance_for_file(std::string const& path);
    };

  }
}

#endif
