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
        ELLE_TRACE("Creating long operation: %s", this->name);
      }

      virtual ~Process()
      {
        ELLE_LOG_COMPONENT("infinit.surface.gap.State");
        ELLE_LOG("Destroying long operation: %s", this->name);

        if (this->thread.joinable())
          this->thread.join();
      }

    private:
      void _run()
      {
        ELLE_LOG_COMPONENT("infinit.surface.gap.State");
        try
          {
            ELLE_TRACE("Running long operation: %s", this->name);
            (this->callback)();
            success = true;
          }
        catch (std::runtime_error const& e)
          {
            ELLE_ERR("process threw an exception");
            this->exception = std::current_exception();
            ELLE_ERR("process threw an exception %s", e.what());
          }
        done = true;
      }
    };

  }
}

#endif
