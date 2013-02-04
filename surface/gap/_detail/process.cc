#include "../State.hh"
#include "Process.hh"

ELLE_LOG_COMPONENT("infinit.surface.gap.State");

namespace surface
{
  namespace gap
  {

    State::ProcessStatus
    State::process_status(ProcessId const id) const
    {
      auto it = _processes.find(id);
      if (it == _processes.end())
        throw elle::Exception{
            "Couldn't find any process with id " + std::to_string(id)
        };
      if (!it->second->done)
        return ProcessStatus::running;
      // the process is terminated.
      if (it->second->success)
        return ProcessStatus::success;
      return ProcessStatus::failure;
    }

    void
    State::process_finalize(ProcessId const id)
    {
      auto it = _processes.find(id);
      if (it == _processes.end())
        throw elle::Exception{
            "Couldn't find any process with id " + std::to_string(id)
        };
      if (!it->second->done)
        throw elle::Exception{"Process not finished"};
      ProcessPtr ptr{it->second.release()};
      _processes.erase(it);
      if (!ptr->success)
        std::rethrow_exception(ptr->exception); // XXX exception_ptr deleted !
    }

    State::ProcessId
    State::_add_process(std::string const& name,
                        std::function<void(void)> const& cb)
    {
      ELLE_TRACE("Adding process.");
      //auto process = elle::make_unique<Process>(*this, name, cb);
      // XXX leak if emplace() fails.
      static ProcessId id = 0;
      _processes[++id].reset(new Process{name, cb});
      return id;
    }
  }
}
