#include "Process.hh"

#include <elle/Buffer.hh>
#include <elle/Exception.hh>
#include <elle/log.hh>
#include <elle/os/getenv.hh>
#include <elle/os/path.hh>

#include <boost/algorithm/string.hpp>

#include <unordered_map>
#include <vector>

#include <errno.h>
#include <signal.h>     // kill
#include <stdlib.h>     // exit, malloc
#include <sys/types.h>
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // fork, close, execvp, environ

#ifdef __APPLE__
extern char** environ;
#endif

#ifndef _GNU_SOURCE // execvpe is a gnu extension
static
int execvpe(char const* binary,
            char* const argv[],
            char* const env[])
{
  std::vector<std::string> paths;
  {
    std::string path = elle::os::getenv("PATH", "/bin:/usr/bin");
    boost::algorithm::split(paths, path, boost::is_any_of(":"));
  }
  std::string binpath;
  for (auto const& path: paths)
  {
    binpath = elle::os::path::join(path, binary);
    if (elle::os::path::exists(binpath))
      {
        (void) ::execve(binpath.c_str(), argv, env);
        if (errno == E2BIG ||
            errno == ENOEXEC ||
            errno == ENOMEM ||
            errno == ETXTBSY)
          return -1;
      }
  }
  return -1;
}
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

    struct ProcessChannel
    {
    public:
      union
      {
        struct { int read_fd; int write_fd; };
        int pipe[2];
      };
      bool borrowed;
      int input_fd;
      int output_fd;

      ProcessChannel()
        : pipe{-1, -1}
        , borrowed{false}
        , input_fd{-1}
        , output_fd{-1}
      {}

      ~ProcessChannel()
      {
        this->close();
      }

      ProcessChannel&
      operator =(ProcessChannel&& other)
      {
        if (this != &other)
        {
          this->close();
          this->read_fd = other.read_fd;
          other.read_fd = -1;
          this->write_fd = other.write_fd;
          other.write_fd = -1;
          other.borrowed = other.borrowed;
        }
        return *this;
      }

      operator bool() const
      {
        return this->read_fd != -1 && this->write_fd != -1;
      }

      ProcessChannel& create()
      {
        if (!*this)
          {
            if (::pipe(this->pipe) < 0)
              throw elle::Exception{"cannot pipe stdin"};
            borrowed = false;
          }
        return *this;
      }

      ProcessChannel& dup2_read(int fd)
      {
        if (this->read_fd == -1)
          throw Exception{"Cannot pipe read endpoint (not initialized)"};
        if (::dup2(this->read_fd, fd) < 0)
          throw Exception{"Cannot dup2 read endpoint"};
        return *this;
      }

      ProcessChannel& dup2_write(int fd)
      {
        if (this->read_fd == -1)
          throw Exception{"Cannot pipe read endpoint (not initialized)"};
        if (::dup2(this->write_fd, fd) < 0)
          throw Exception{"Cannot dup2 read endpoint"};
        return *this;
      }

      ProcessChannel& close()
      {
        this->close_read();
        this->close_write();
        return *this;
      }

      ProcessChannel& close_read()
      {
        if (this->read_fd != -1)
        {
          if (!borrowed)
            (void) ::close(this->read_fd);
          this->read_fd = -1;
        }
        return *this;
      }

      ProcessChannel& close_write()
      {
        if (this->write_fd != -1)
        {
          if (!this->borrowed)
            (void) ::close(this->write_fd);
          this->write_fd = -1;
        }
        return *this;
      }

      ProcessChannel&
      pipe_with(ProcessChannel& other)
      {
        other.close();
        this->create();
        other.read_fd = this->read_fd;
        other.write_fd = this->write_fd;
        other.borrowed = true;
        return *this;
      }

    };

    //- ProcessConfig ---------------------------------------------------------

    struct ProcessConfig::Impl
    {
    public:
      bool            daemon;
      ProcessChannel  channels[3];

    public:
      typedef std::unordered_map<std::string, std::string> EnvMap;
      EnvMap          env;

    public:
      Impl()
        : daemon{false}
        , channels()
      {
        this->channels[(int) ProcessChannelStream::in].input_fd = STDIN_FILENO;
        this->channels[(int) ProcessChannelStream::out].output_fd = STDOUT_FILENO;
        this->channels[(int) ProcessChannelStream::err].output_fd = STDERR_FILENO;
      }

      ~Impl()
      {}

    public:
      // Merge config implementation.
      void update(Impl& other)
      {
        this->daemon = other.daemon;
        for (auto const& pair: other.env)
          this->env[pair.first] = pair.second;
        for (unsigned int i = 0;
             i < sizeof(this->channels) / sizeof(this->channels[0]);
             i++)
          {
            if (other.channels[i]) // override only if valid channel
              this->channels[i] = std::move(other.channels[i]);
          }
      }
    };

    ProcessConfig::ProcessConfig()
      : _impl{new Impl{}}
    {}

    ProcessConfig::ProcessConfig(ProcessConfig&& other)
      : _impl{std::move(other._impl)}
    {}

    ProcessConfig&
    ProcessConfig::operator =(ProcessConfig&& other)
    {
      if (this != &other)
          _impl = std::move(other._impl);
      return *this;
    }

    ProcessConfig::~ProcessConfig()
    {
    }

    ProcessConfig&
    ProcessConfig::update(ProcessConfig&& other)
    {
      if (this == &other)
        return *this;
      this->_impl->update(*other._impl);
      return *this;
    }


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
          {
            continue;
          }
          std::string val = values[1];
          for (unsigned int i = 2; i < values.size(); ++i)
            val += "=" + values[i];
          _impl->env[values[0]] = val;
        }
      return *this;
    }

    bool
    ProcessConfig::has_pipe(ProcessChannelStream const channel)
    {
      return this->channel(channel); // bool cast
    }

    ProcessConfig&
    ProcessConfig::create_pipe(ProcessChannelStream const channel_)
    {
      this->channel(channel_).create();
      return *this;
    }

    ProcessConfig&
    ProcessConfig::connect_stdout(ProcessConfig& other)
    {
      this->channel(ProcessChannelStream::out).pipe_with(
          other.channel(ProcessChannelStream::in)
      );
      return *this;
    }

    ProcessChannel&
    ProcessConfig::channel(ProcessChannelStream const channel)
    {
      return _impl->channels[static_cast<int>(channel)];
    }

    ProcessConfig& ProcessConfig::merge_stderr()
    {
      throw elle::Exception{"Not implemented"};
      return *this;
    }

    ProcessConfig
    process_config(ProcessConfigModel const model)
    {
      ProcessConfig config;
      if (model & daemon_config)
        config.daemon(true);
      if (model & inherit_environment_config)
        config.inherit_current_environment();
      if (model & pipe_stdin_config)
        config.create_pipe(ProcessChannelStream::in);
      if (model & pipe_stdout_config)
        config.create_pipe(ProcessChannelStream::out);
      if (model & pipe_stderr_config)
        config.create_pipe(ProcessChannelStream::err);
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
    {
      ELLE_TRACE("Launching %s %s", binary, arguments);
      pid_t binary_pid = ::fork();

      if (binary_pid < 0)
        throw elle::Exception{"Cannot fork"};

      if (binary_pid == 0) // child process
        {
          char const** exec_args = new char const*[arguments.size() + 2];
          unsigned int i = 0;
          exec_args[i++] = binary.c_str();
          for (auto const& arg : arguments)
            exec_args[i++] = arg.c_str();
          exec_args[i] = nullptr;


          char** envp = new char*[_config._impl->env.size() + 1];
          i = 0;
          for (auto const& pair: _config._impl->env)
            {
              std::string s = pair.first + "=" + pair.second;
              envp[i++] = strdup(s.c_str());
            }
          envp[i] = nullptr;


          static ProcessChannelStream const streams[] = {
            ProcessChannelStream::in,
            ProcessChannelStream::out,
            ProcessChannelStream::err,
          };
          for (auto const stream: streams)
          {
            if (_config.has_pipe(stream))
            {
              ProcessChannel& channel = _config.channel(stream);
              if (channel.input_fd != -1)
              {
                channel.dup2_read(channel.input_fd);
                channel.close_read();
              }

              if (channel.output_fd != -1)
              {
                channel.dup2_write(channel.output_fd);
                channel.close_write();
              }
            }
          }

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
          _config.channel(ProcessChannelStream::in).close_read();
          _config.channel(ProcessChannelStream::out).close_write();
          _config.channel(ProcessChannelStream::err).close_write();
        }
    }

    Process::Process(ProcessConfigModel const model,
                     std::string const& binary,
                     std::list<std::string> const& arguments)
      : Process{process_config(model), binary, arguments}
    {}

    Process::Process(std::string const& binary,
                     std::list<std::string> const& arguments)
      : Process{normal_config, binary, arguments}
    {}

    Process::Process(std::string const& binary)
      : Process{binary, {}}
    {}

    Process::Process(Process&& other)
      : _impl{std::move(other._impl)}
      , _config{std::move(other._config)}
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
      int fd = _config.channel(ProcessChannelStream::out).read_fd;
      if (fd == -1)
        throw elle::Exception{"stdout not piped"};
      elle::Buffer buf(max);
      ssize_t read_bytes = ::read(fd, buf.mutable_contents(), max);
      if (read_bytes < 0)
        throw elle::Exception{"cannot read from stdout pipe"};
      return std::string((char const*) buf.contents(), read_bytes);
    }

    template <typename Implem>
    ProcessCommandConcept<Implem>::ProcessCommandConcept()
    {}
    template <typename Implem>
    ProcessCommandConcept<Implem>::~ProcessCommandConcept()
    {}

    template
    struct ProcessCommandConcept<detail::Command>;

    namespace detail
    {

      Process& Command::process()
      {
        if (_process == nullptr)
          throw elle::Exception{
            "cannot access process of a not executed command"
          };
        return *_process;
      }

      Command& Command::wait()
      {
        if (_process == nullptr)
          throw elle::Exception{"cannot wait a command not executed"};
        this->_process->wait();
        if (this->_next != nullptr)
          this->_next->wait();
        return *this;
      }

      Process::StatusCode
      Command::wait_status()
      {
        if (_process == nullptr)
          throw elle::Exception{"cannot wait a command not executed"};
        Process::StatusCode status = this->_process->wait_status();
        if (this->_next != nullptr)
        {
          Process::StatusCode next_status = this->_next->wait_status();
          if (next_status != 0)
            status = next_status;
        }
        return status;
      }

      Command& Command::execute()
      {
        if (_process != nullptr)
          throw elle::Exception{"already executed"};
        if (this->_binary.empty())
          throw elle::Exception{"Cannot execute a process without any binary name"};
        _process.reset(new Process{
          std::move(this->_config),
          this->_binary,
          this->_args
        });
        if (this->_next != nullptr)
          this->_next->execute();
        return *this;
      }
    }

  }
}

