#include "Process.hh"

#include <elle/Buffer.hh>
#include <elle/Exception.hh>
#include <elle/log.hh>

#include <boost/algorithm/string.hpp>

#include <unordered_map>

#include <signal.h>     // kill
#include <stdlib.h>     // exit, malloc
#include <sys/types.h>
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // fork, close, execvp, environ
#ifdef __apple__
# include <environ.h>
#endif

ELLE_LOG_COMPONENT("elle.system.Process");

namespace elle
{
  namespace system
  {

    enum
    {
      READ_ENDPOINT = 0,
      WRITE_ENDPOINT = 1,
    };

    //- ProcessConfig ---------------------------------------------------------

    struct ProcessConfig::Impl
    {
      bool        daemon;
      int         in_pipe[2];
      int         out_pipe[2];
      int         err_pipe[2];

      typedef std::unordered_map<std::string, std::string> EnvMap;
      EnvMap      env;

      Impl()
        : daemon{false}
        , in_pipe{-1, -1}
        , out_pipe{-1, -1}
        , err_pipe{-1, -1}
      {}
    };

    ProcessConfig::ProcessConfig()
      : _impl{new Impl{}}
    {}

    ProcessConfig::ProcessConfig(ProcessConfig&& other)
      : _impl{std::move(other._impl)}
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

    std::string const&
    ProcessConfig::getenv(std::string const& name) const
    {
      return _impl->env[name];
    }

    ProcessConfig&
    ProcessConfig::setenv(std::string const& name,
                          std::string const& value)
    {
      _impl->env[name] = value;
      return *this;
    }

    ProcessConfig&
    ProcessConfig::inherit_current_environment()
    {
      char** envp = environ;
      for (; *envp != nullptr; envp++)
        {
          std::vector<std::string> values;
          boost::split(values, *envp, boost::is_any_of("="));
          if (values.size() < 2)
            continue;
          std::string val = values[1];
          for (unsigned int i = 2; i < values.size(); ++i)
            val += "=" + values[i];
          _impl->env[values[0]] = val;
        }
      return *this;
    }

    ProcessConfig& ProcessConfig::pipe_stdin()
    {
      if (_impl->in_pipe[0] != -1)
        throw elle::Exception{"stdin already piped"};
      if (::pipe(_impl->in_pipe) < 0)
        throw elle::Exception{"cannot pipe stdin"};
      return *this;
    }

    ProcessConfig& ProcessConfig::pipe_stderr()
    {
      if (_impl->err_pipe[0] != -1)
        throw elle::Exception{"stderr already piped"};
      if (::pipe(_impl->err_pipe) < 0)
        throw elle::Exception{"cannot pipe stderr"};
      return *this;
    }

    ProcessConfig& ProcessConfig::pipe_stdout()
    {
      if (_impl->out_pipe[0] != -1)
        throw elle::Exception{"stdout already piped"};
      if (::pipe(_impl->out_pipe) < 0)
        throw elle::Exception{"cannot pipe stdout"};
      return *this;
    }

    ProcessConfig& ProcessConfig::merge_stderr()
    {
      throw elle::Exception{"Not implemented"};
      return *this;
    }

    ProcessConfig
    process_config(ProcessKind const kind)
    {
      ProcessConfig config;
      if (kind & daemon_config)
        config.daemon(true);
      if (kind & normal_config)
        config.inherit_current_environment();
      if (kind & check_output_config)
        config.pipe_stdout();
      return config;
    }

    //- Process ---------------------------------------------------------------

    struct Process::Impl
    {
      std::string             binary;
      std::list<std::string>  arguments;
      pid_t                   pid;
      int                     status;
    };

    Process::Process(ProcessConfig&& config_,
                     std::string const& binary,
                     std::list<std::string> const& arguments)
      : _impl{new Impl{binary, arguments, 0, 0}}
      , _config{std::move(config_)}
      , _config_impl{_config._impl.get()}
    {
      ELLE_ASSERT(_config_impl != nullptr);
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


          char** envp = new char*[_config_impl->env.size() + 1];
          i = 0;
          for (auto const& pair: _config_impl->env)
            {
              std::string s = pair.first + "=" + pair.second;
              envp[i] = strdup(s.c_str());
            }
          envp[i] = nullptr;



          if (_config_impl->in_pipe[0] != -1) // is initialized
            {
              if (::dup2(_config_impl->in_pipe[READ_ENDPOINT], STDIN_FILENO) < 0)
                throw elle::Exception{"Cannot duplicate stdin"};
              ::close(_config_impl->in_pipe[WRITE_ENDPOINT]);
            }

          if (_config_impl->err_pipe[0] != -1) // is initialized
            {
              if (::dup2(_config_impl->err_pipe[WRITE_ENDPOINT], STDERR_FILENO) < 0)
                throw elle::Exception{"Cannot duplicate stderr"};
              ::close(_config_impl->err_pipe[READ_ENDPOINT]);
            }

          if (_config_impl->out_pipe[0] != -1) // is initialized
            {
              if (::dup2(_config_impl->out_pipe[WRITE_ENDPOINT], STDOUT_FILENO) < 0)
                throw elle::Exception{"Cannot duplicate stdout"};
              ::close(_config_impl->out_pipe[READ_ENDPOINT]);
            }

          //close(STDERR_FILENO);
          if (::execvpe(binary.c_str(), (char**) exec_args, envp) != 0)
            {
              ELLE_ERR("Cannot execvp %s", binary.c_str());
            }
          ::exit(EXIT_FAILURE);
        }
      else // parent process
        {
          ELLE_DEBUG("Binary %s %s has pid %d", binary, arguments, binary_pid);
          _impl->pid = binary_pid;
          if (_config_impl->in_pipe[0] != -1) // is initialized
            ::close(_config_impl->in_pipe[READ_ENDPOINT]);

          if (_config_impl->err_pipe[0] != -1) // is initialized
            ::close(_config_impl->err_pipe[WRITE_ENDPOINT]);

          if (_config_impl->out_pipe[0] != -1) // is initialized
            ::close(_config_impl->out_pipe[WRITE_ENDPOINT]);
        }
    }

    Process::Process(ProcessKind const kind,
                     std::string const& binary,
                     std::list<std::string> const& arguments)
      : Process{process_config(kind), binary, arguments}
    {}

    Process::Process(std::string const& binary,
                     std::list<std::string> const& arguments)
      : Process{normal_config, binary, arguments}
    {}

    Process::Process(std::string const& binary)
      : Process{binary, {}}
    {}

    Process::Process(Process&& other):
      _impl{std::move(other._impl)}
    {}

    Process::~Process()
    {
      if (!_impl)
        return;

      if (_impl->pid == 0 || _config.daemon())
        return;

      try
        {
          if (this->running())
            {
              ELLE_WARN("Killing a running process %s(%d)",
                        _impl->binary, _impl->pid);
              this->kill(ProcessTermination::wait);
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

    Process::StatusCode
    Process::status(ProcessTermination const term)
    {
      ELLE_TRACE("Waiting for binary %s %s", _impl->binary, _impl->arguments);
      if (_impl->pid == 0)
        return _impl->status;

      int status;
      int options = (term == ProcessTermination::dont_wait ? WNOHANG : 0);
      pid_t ret = ::waitpid(_impl->pid, &status, options);
      if (ret < 0)
        throw elle::Exception{"Cannot waitpid"};

      // No child exited and WNOHANG specified.
      if (ret == 0 && term == ProcessTermination::dont_wait)
        return 0;

      // ret == _impl->pid is true

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
      else if (term == ProcessTermination::wait)
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

    Process::StatusCode
    Process::wait_status()
    {
      return this->status(ProcessTermination::wait);
    }

    void Process::wait()
    {
      if (this->wait_status() != 0)
        throw elle::Exception{
          "The program '" + _impl->binary +
          "' failed exited with error code: " + std::to_string(_impl->status)
        };
    }

    void Process::kill(ProcessTermination const term)
    {
      if (_impl->pid == 0)
        return;

      ::kill(_impl->pid, SIGKILL);
      if (term == ProcessTermination::wait)
        this->wait_status();
    }

    void Process::terminate(ProcessTermination const term)
    {
      if (_impl->pid == 0)
        return;

      ::kill(_impl->pid, SIGTERM);
      if (term == ProcessTermination::wait)
        this->wait_status();
    }

      std::string
      Process::read(size_t const max)
      {
        int fd = _config_impl->out_pipe[READ_ENDPOINT];
        if (fd == -1)
          throw elle::Exception{"stdout not piped"};
        elle::Buffer buf(max);
        ssize_t read_bytes = ::read(fd, buf.mutable_contents(), max);
        if (read_bytes < 0)
          throw elle::Exception{"cannot read from stdout pipe"};
        return std::string((char const*) buf.contents(), read_bytes);
      }
  }
}

