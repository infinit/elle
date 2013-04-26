#include "Process.hh"

#include <elle/Buffer.hh>
#include <elle/Exception.hh>
#include <elle/log.hh>
#include <elle/os/getenv.hh>
#include <elle/os/path.hh>
#include <elle/os/environ.hh>
#include <elle/print.hh>

#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

#include <algorithm>
#include <unordered_map>
#include <vector>

#include <errno.h>
#include <signal.h>     // kill
#include <stdlib.h>     // exit, malloc
#include <sys/types.h>
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // fork, close, execvp, environ

ELLE_LOG_COMPONENT("elle.system.Process");

#ifndef _GNU_SOURCE // execvpe is a gnu extension
static
int execvpe(char const* binary,
            char* const argv[],
            char* const env[])
{
  if (binary == nullptr or binary[0] == '\0')
    return -1;

  // absolute or relative path
  if (binary[0] == '.' or binary[0] == '/')
  {
    (void) ::execve(binary, argv, env);
    return -1;
  }

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

      explicit
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

      ProcessChannel&
      create()
      {
        if (!*this)
          {
            if (::pipe(this->pipe) < 0)
              throw elle::Exception{"cannot pipe stdin"};
            borrowed = false;
          }
        return *this;
      }

      ProcessChannel&
      dup2_read(int fd)
      {
        if (this->read_fd == -1)
          throw Exception{"cannot pipe read endpoint (not initialized)"};
        if (::dup2(this->read_fd, fd) < 0)
          throw Exception{"cannot dup2 read endpoint"};
        return *this;
      }

      ProcessChannel&
      dup2_write(int fd)
      {
        if (this->read_fd == -1)
          throw Exception{"cannot pipe read endpoint (not initialized)"};
        if (::dup2(this->write_fd, fd) < 0)
          throw Exception{"cannot dup2 read endpoint"};
        return *this;
      }

      ProcessChannel&
      close()
      {
        this->close_read();
        this->close_write();
        return *this;
      }

      ProcessChannel&
      close_read()
      {
        if (this->read_fd != -1)
        {
          if (!borrowed)
            (void) ::close(this->read_fd);
          this->read_fd = -1;
        }
        return *this;
      }

      ProcessChannel&
      close_write()
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
      FILE*           files[3];

    public:
      typedef std::unordered_map<std::string, std::string> EnvMap;
      EnvMap          env;

    public:
      Impl()
        : daemon{false}
        , channels()
        , files{nullptr, nullptr, nullptr}
      {
        this->channels[(int) ProcessChannelStream::in].input_fd = STDIN_FILENO;
        this->channels[(int) ProcessChannelStream::out].output_fd = STDOUT_FILENO;
        this->channels[(int) ProcessChannelStream::err].output_fd = STDERR_FILENO;
      }

      ~Impl()
      {
        for (unsigned int i = 0; i < 3; ++i)
        {
          if (this->files[i] != nullptr)
          {
            ::fclose(this->files[i]);
            this->files[i] = nullptr;
          }
        }
      }

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
      : _this{new Impl{}}
    {}

    ProcessConfig::ProcessConfig(ProcessConfig&& other)
      : _this{std::move(other._this)}
    {}

    ProcessConfig&
    ProcessConfig::operator =(ProcessConfig&& other)
    {
      if (this != &other)
          _this = std::move(other._this);
      return *this;
    }

    ProcessConfig::~ProcessConfig()
    {}

    ProcessConfig&
    ProcessConfig::update(ProcessConfig&& other)
    {
      if (this == &other)
        return *this;
      _this->update(*other._this);
      return *this;
    }


    bool
    ProcessConfig::daemon() const
    { return _this->daemon; }

    ProcessConfig&
    ProcessConfig::daemon(bool mode)
    {
      _this->daemon = mode;
      return *this;
    }

    std::string const&
    ProcessConfig::getenv(std::string const& name) const
    {
      return _this->env[name];
    }

    ProcessConfig&
    ProcessConfig::setenv(std::string const& name,
                          std::string const& value)
    {
      _this->env[name] = value;
      return *this;
    }

    ProcessConfig&
    ProcessConfig::inherit_current_environment()
    {
      for (auto const& pair: elle::os::environ())
        _this->env.insert(pair);
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

    ProcessConfig&
    ProcessConfig::pipe_file(ProcessChannelStream const channel,
                             std::string const& path,
                             std::string const& mode)
    {
      auto& chan = this->channel(channel);
      auto& file = _this->files[(int) channel];
      if (file != nullptr)
        throw elle::Exception{"Already piped"};
      file = ::fopen(path.c_str(), mode.c_str());
      if (file == nullptr)
        throw elle::Exception{"Cannot open file at '" + path + "'"};
      try
      {
        if (channel == ProcessChannelStream::in)
          chan.create().read_fd = ::fileno(file);
        else
          chan.create().write_fd = ::fileno(file);
        ELLE_ASSERT(this->has_pipe(channel));
      }
      catch (...)
      {
        ::fclose(file);
        file = nullptr;
        throw;
      }
      return *this;
    }

    ProcessChannel&
    ProcessConfig::channel(ProcessChannelStream const channel)
    {
      return _this->channels[static_cast<int>(channel)];
    }

    ProcessConfig&
    ProcessConfig::merge_stderr()
    {
      throw elle::Exception{"not implemented"};
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
      : _this{new Impl{binary, arguments, 0, 0}}
      , _config{std::move(config_)}
    {
      ELLE_TRACE("launching %s %s", binary, arguments);

      pid_t binary_pid = ::fork();
      // XXX Do not log from the child process (see /tests/system/fork-log-fail)

      if (binary_pid < 0)
        throw elle::Exception{"cannot fork"};

      if (binary_pid == 0) // child process
        {
          char const** exec_args = new char const*[arguments.size() + 2];
          unsigned int i = 0;
          exec_args[i++] = binary.c_str();
          for (auto const& arg : arguments)
            exec_args[i++] = arg.c_str();
          exec_args[i] = nullptr;

          char** envp = new char*[_config._this->env.size() + 1];
          i = 0;
          for (auto const& pair: _config._this->env)
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
              std::cerr << "couldn't execvp: " << binary << std::endl;
            }
          ::exit(EXIT_FAILURE);
        }
      else // parent process
        {
          ELLE_DEBUG("child process pid: %s", binary_pid);
          ELLE_ASSERT_NEQ(binary_pid, ::getpid());
          _this->pid = binary_pid;
          _config.channel(ProcessChannelStream::in).close_read();
          _config.channel(ProcessChannelStream::out).close_write();
          _config.channel(ProcessChannelStream::err).close_write();

          std::vector<std::string> procs;

          std::string env = elle::os::getenv("ELLE_PROCESS_DEBUG", "");
          if (!env.empty())
          {
            boost::algorithm::split(procs, env, boost::is_any_of(","));
            auto it = std::find_if(
                        begin(procs),
                        end(procs),
                        [&] (std::string const& s) {
                          return boost::algorithm::ends_with(binary, s);
                        });
            if (it != end(procs))
            {
              std::string cmd =
                elle::os::getenv("ELLE_PROCESS_DEBUG_CMD", "");
              if (!cmd.empty())
              {
                // XXX: FIXME: we need to make this extensible
                ::system(elle::sprintf(cmd.c_str(), this->id()).c_str());
              }
            }
          }
        }
      auto status = this->status(); // This ensure at least one waitpid is done
      ELLE_DEBUG("binary %s %s has pid %d and status %d",
                 binary, arguments, binary_pid, status);
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
      : _this{std::move(other._this)}
      , _config{std::move(other._config)}
    {}

    Process::~Process()
    {
      if (!_this)
        return;

      if (_this->pid == 0 || _config.daemon())
        return;

      try
        {
          ELLE_WARN("destroying a running process %s (%s)",
                    _this->binary, _this->pid);
          if (this->running())
            {
              ELLE_WARN("killing a running process %s(%d)",
                        _this->binary, _this->pid);
              this->kill(ProcessTermination::wait);
            }
        }
      catch (std::runtime_error const& err)
        {
          ELLE_WARN("got an exception while killing %s(%d): %s",
                    _this->binary, _this->pid, err.what());
        }
      catch (...)
        {
          ELLE_WARN("got an unknown error while killing %s(%d)",
                    _this->binary, _this->pid);
        }
    }

    Process::StatusCode
    Process::status(ProcessTermination const term)
    {
      if (_this->pid == 0)
      {
        ELLE_DEBUG("%s (pid = 0): status %s", _this->binary, _this->status);
        return _this->status;
      }
      ELLE_DEBUG("%s for binary (%s) %s %s",
                  (term == ProcessTermination::dont_wait ? "checking"
                                                         : "waiting"),
                 _this->pid, _this->binary, _this->arguments);

      int status_ = 0;
      int options = (term == ProcessTermination::dont_wait ? WNOHANG : 0);
      pid_t ret = 0;
      do
      {
        ret = ::waitpid(_this->pid, &status_, options);
      } while (ret < 0 && errno == EINTR);

      if (ret < 0)
        {
          if (errno == ECHILD)
            {
              ELLE_WARN(
                "the pid %d is no longer valid, exit status set to 1 (failure)",
                _this->pid
              );
              _this->pid = 0;
              _this->status = EXIT_FAILURE;
              return _this->status;
            }
          std::string reason;
          if (errno == EFAULT)
            reason = "status argument points to an illegal address";
          else if (errno == EINVAL)
            reason = "invalid or undefined flags are passed in the options argument";
          else
            reason = elle::sprint("unknown errno value:", errno);
          throw elle::Exception{"Cannot waitpid: " + reason};
        }

      ELLE_DEBUG("binary %s successfully waited", _this->binary);
      // No child exited and WNOHANG specified.
      if (ret == 0 && term == ProcessTermination::dont_wait)
        return 0;

      // ret == _this->pid is true

      if (WIFEXITED(status_))
        {
          _this->status = WEXITSTATUS(status_);
          ELLE_DEBUG("binary %s %s exited with status %d",
                     _this->binary, _this->arguments, _this->status);
          if (_this->status < 0) // ensure positive value.
            {
              ELLE_WARN("The process %s exited with a negative value: %d",
                        _this->binary, _this->status);
              _this->status = -_this->status;
            }
          _this->pid = 0;
        }
      else if (WIFSIGNALED(status_))
        {
          _this->status = -WTERMSIG(status_);
          ELLE_DEBUG("binary %s %s signaled with status %d",
                     _this->binary, _this->arguments, -_this->status);
          if (_this->status > 0) // ensure negative value.
            {

              ELLE_WARN("the process %s signaled with a negative value: %d",
                        _this->binary, -_this->status);
              _this->status = -_this->status;
            }
          _this->pid = 0;
        }
      else if (term == ProcessTermination::wait)
        {
          throw elle::Exception{"unexpected program termination"};
        }
      return _this->status;
    }

    bool
    Process::running()
    {
      // The order is important: status() has the opportunity to set the pid to
      // 0 if the program has exited.
      return (this->status() == 0 && _this->pid != 0);
    }

    Process::StatusCode
    Process::wait_status(Milliseconds timeout)
    {
      ELLE_DEBUG("waiting status of %s", _this->binary);
      if (timeout == Milliseconds(0))
        return this->status(ProcessTermination::wait);

      StatusCode res = this->status();
      while (timeout > Milliseconds(0) && _this->pid != 0)
      {
        boost::this_thread::sleep_for(boost::chrono::nanoseconds(1000));
        timeout -= Milliseconds(1);
        res = this->status();
      }
      return res;
    }

    void
    Process::wait(Milliseconds timeout)
    {
      if (this->wait_status(timeout) != 0)
        throw elle::Exception{
          "the program '" + _this->binary +
          "' failed exited with error code: " + std::to_string(_this->status)
        };
    }

    Process::Id
    Process::id() const
    {
      return _this->pid;
    }

    void
    Process::kill(ProcessTermination const term)
    {
      if (_this->pid == 0)
        return;

      ::kill(_this->pid, SIGKILL);
      if (term == ProcessTermination::wait)
        this->wait_status();
    }

    void
    Process::_signal(int signal, ProcessTermination const term)
    {
      ELLE_DEBUG_SCOPE("about to signal pid %s with %s", _this->pid, signal);

      if (_this->pid == 0)
        return;

      ::kill(_this->pid, signal);

      if (term == ProcessTermination::wait)
      {
        ELLE_DEBUG("waiting pid %s", _this->pid);
        this->wait_status();
      }
    }

    void
    Process::interrupt(ProcessTermination const term)
    {
      this->_signal(SIGINT, term);
    }

    void
    Process::terminate(ProcessTermination const term)
    {
      this->_signal(SIGTERM, term);
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

      Process&
      Command::process()
      {
        if (_process == nullptr)
          throw elle::Exception{
            "cannot access process of a not executed command"
          };
        return *_process;
      }

      Command&
      Command::wait()
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

      Command&
      Command::execute()
      {
        if (_process != nullptr)
          throw elle::Exception{"already executed"};
        if (this->_binary.empty())
          throw elle::Exception{
            "cannot execute a process without any binary name"
          };
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
