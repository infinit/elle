#ifndef  PLASMA_WATCHDOG_WATCHDOGINTERFACE_HH
# define PLASMA_WATCHDOG_WATCHDOGINTERFACE_HH

# include <string>
# include <vector>

namespace plasma
{
  namespace watchdog
  {
    class Application;

    class WatchdogInterface
    {
    public:
      struct Directory
      {
        enum class Status {Ok, NotSynchronized, Bad, Ugly};
        Status status;
        std::string name;
      };

    private:
      Application& _app;

    public:
      WatchdogInterface(Application& app);
      std::vector<Directory> DirListing(std::string const& abspath);
    };

  }
}

#endif /* ! WATCHDOGINTERFACE_HH */
