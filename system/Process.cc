#include "Process.hh"

#include <elle/log.hh>
#include <elle/Exception.hh>

#include <signal.h>     // kill
#include <stdlib.h>     // exit, malloc
#include <sys/types.h>
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // fork, close, execvp

ELLE_LOG_COMPONENT("elle.system.Process");

namespace elle
{
  namespace system
  {

    //- ProcessConfig ---------------------------------------------------------

    struct ProcessConfig::Impl
    {
      bool daemon;

      Impl()
        : daemon{false}
      {}
    };

    ProcessConfig::ProcessConfig():
      _impl{new Impl}
    {}

    ProcessConfig::~ProcessConfig()
    {}

    bool
    ProcessConfig::daemon() const
    { return _impl->daemon; }

    ProcessConfig&
    ProcessConfig::daemon(bool mode)
    {
      _impl->daemon = mode;
      return *this;
    }

    ProcessConfig const&
    process_config(Process::Kind const kind)
    {
      static std::unique_ptr<ProcessConfig> normal_config;
      static std::unique_ptr<ProcessConfig> daemon_config;
      throw std::runtime_error{"Not implemented"};
    }

    //- Process ---------------------------------------------------------------

    struct Process::Impl
    {
      Kind const              kind;
      std::string             binary;
      std::list<std::string>  arguments;
      pid_t                   pid;
      int                     status;
    };

    Process::Process(Kind const kind,
                     std::string const& binary,
                     std::list<std::string> const& arguments):
      _impl{new Impl{kind, binary, arguments, 0, 0}}
    {
      ELLE_TRACE("Launching %s %s", binary, arguments);
      pid_t binary_pid = ::fork();

      if (binary_pid < 0)
        throw elle::Exception{"Cannot fork"};

      if (binary_pid == 0) // child process
        {
          char const** exec_args = new char const*[arguments.size() + 1];
          unsigned int i = 0;
          exec_args[i++] = binary.c_str();
          for (auto const& arg : arguments)
            exec_args[i++] = arg.c_str();
          exec_args[i] = nullptr;

          //close(STDERR_FILENO);
          if (::execvp(binary.c_str(), (char**) exec_args) != 0)
            {
              ELLE_ERR("Cannot execvp %s", binary.c_str());
            }
          ::exit(EXIT_FAILURE);
        }
      else // parent process
        {
          ELLE_DEBUG("Binary %s %s has pid %d", binary, arguments, binary_pid);
          _impl->pid = binary_pid;
        }
    }

    Process::Process(std::string const& binary,
                     std::list<std::string> const& arguments):
      Process(Kind::normal, binary, arguments)
    {}

    Process::Process(Process&& other):
      _impl{std::move(other._impl)}
    {}

    Process::~Process()
    {
      if (!_impl)
        return;

      if (_impl->pid == 0 || _impl->kind == Kind::daemon)
        return;

      try
        {
          if (this->running())
            {
              ELLE_WARN("Killing a running process %s(%d)",
                        _impl->binary, _impl->pid);
              this->kill(Termination::wait);
            }
        }
      catch (std::runtime_error const& err)
        {
          ELLE_WARN("Got an exception while killing %s(%d): %s",
                    _impl->binary, _impl->pid, err.what());
        }
      catch (...)
        {
          ELLE_WARN("Got a unknown error while killing %s(%d)",
                    _impl->binary, _impl->pid);
        }
    }

    int
    Process::status(Termination const term)
    {
      ELLE_TRACE("Waiting for binary %s %s", _impl->binary, _impl->arguments);
      if (_impl->pid == 0)
        return _impl->status;

      int status;
      int options = (term == Termination::dont_wait ? WNOHANG : 0);
      if (::waitpid(_impl->pid, &status, options) < 0)
        throw elle::Exception{"Cannot waitpid"};
      if (WIFEXITED(status))
        {
          _impl->status = WEXITSTATUS(status);
          ELLE_DEBUG("Binary %s %s exited with status %d",
                     _impl->binary, _impl->arguments, _impl->status);
          if (_impl->status < 0) // ensure positive value.
            {
              ELLE_WARN("The process %s exited with a negative value: %d",
                        _impl->binary, _impl->status);
              _impl->status = -_impl->status;
            }
          _impl->pid = 0;
        }
      else if (WIFSIGNALED(status))
        {
          _impl->status = -WTERMSIG(status);
          ELLE_DEBUG("Binary %s %s signaled with status %d",
                     _impl->binary, _impl->arguments, -_impl->status);
          if (_impl->status > 0) // ensure negative value.
            {

              ELLE_WARN("The process %s signaled with a negative value: %d",
                        _impl->binary, -_impl->status);
              _impl->status = -_impl->status;
            }
          _impl->pid = 0;
        }
      else if (term == Termination::wait)
        {
          throw elle::Exception{"Unexpected program termination"};
        }
      return _impl->status;
    }

    bool
    Process::running()
    {
      // The order is important: status() has the opportunity to set the pid to
      // 0 if the program has exited.
      return (this->status() == 0 && _impl->pid != 0);
    }

    int
    Process::wait_status()
    {
      return this->status(Termination::wait);
    }

    void Process::wait()
    {
      if (this->wait_status() != 0)
        throw elle::Exception{
          "The program '" + _impl->binary +
          "' failed exited with error code: " + std::to_string(_impl->status)
        };
    }

    void Process::kill(Termination const term)
    {
      if (_impl->pid == 0)
        return;

      ::kill(_impl->pid, SIGKILL);
      if (term == Termination::wait)
        this->wait_status();
    }

    void Process::terminate(Termination const term)
    {
      if (_impl->pid == 0)
        return;

      ::kill(_impl->pid, SIGTERM);
      if (term == Termination::wait)
        this->wait_status();
    }

  }
}

