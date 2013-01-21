#ifndef  SURFACE_GAP_STATE_PROCESS_HH
# define SURFACE_GAP_STATE_PROCESS_HH

# include "../State.hh"

# include <elle/log.hh>

# include <functional>
# include <stdexcept>
# include <string>
# include <thread>

namespace surface
{
  namespace gap
  {

    struct State::Process
    {
      typedef std::function<void(void)> Callback;

      Callback            callback;
      std::string         name;
      bool                done;
      bool                success;
      std::exception_ptr  exception;
      std::thread         thread;

      Process(std::string const& name,
              Callback const& cb)
        : callback{cb}
        , name{name}
        , done{false}
        , success{false}
        , exception{}
        , thread{&Process::_run, this}
      {
        ELLE_LOG_COMPONENT("infinit.surface.gap.State");
        ELLE_LOG("Creating long operation: %s", this->name);
      }

    private:
      void _run()
      {
        ELLE_LOG_COMPONENT("infinit.surface.gap.State");
        try
          {
            ELLE_LOG("Running long operation: %s", this->name);
            (this->callback)();
            success = true;
          }
        catch (...)
          {
            this->exception = std::current_exception();
          }
        done = true;
      }
    };

  }
}

#endif
