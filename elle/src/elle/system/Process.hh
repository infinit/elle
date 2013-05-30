#ifndef  ELLE_SYSTEM_PROCESS_HH
# define ELLE_SYSTEM_PROCESS_HH

# include <iosfwd>
# include <list>
# include <memory>
# include <string>
# include <chrono>

namespace elle
{
  namespace system
  {

    /// Shortcuts for common process configurations.
    enum ProcessConfigModel
    {
      /// Environment is empty.
      empty_config = 0,

      /// All environment variables inherited.
      inherit_environment_config = 1 << 1,

      /// alias for inherit_environment_config
      normal_config = inherit_environment_config,

      /// Detach from parent.
      daemon_config = 1 << 2,

      /// Allow connection between a program input/output and another program,
      /// a file, or any file descriptor.
      pipe_stdin_config = 1 << 3,
      pipe_input_config = pipe_stdin_config,
      pipe_stdout_config = 1 << 4,
      pipe_stderr_config = 1 << 5,
      pipe_output_config = pipe_stdout_config | pipe_stderr_config,

      /// alias for normal config and output piped.
      check_output_config = normal_config | pipe_output_config,
    };


    /// Flags for process actions.
    enum class ProcessTermination { wait, dont_wait, };

    /// Input and output streams for a process.
    enum class ProcessChannelStream { in, out, err, };

    // Internal structure representing a process' channel.
    struct ProcessChannel;

    /// @brief Customize a process launch.
    ///
    /// Store process environment and its input/output configuration. This
    /// class is usually generated from @a ProcessConfigModel template.
    ///
    /// @see @a process_config() function.
    class ProcessConfig
    {
    private:
      struct Impl;
      std::unique_ptr<Impl> _this;

    public:
      /// Default constructible.
      ProcessConfig();

      /// Moveable.
      ProcessConfig(ProcessConfig&& other);
      ProcessConfig&
      operator =(ProcessConfig&& other);

      ~ProcessConfig();

      /// Update config by merging another one.
      ProcessConfig&
      update(ProcessConfig&& other);

      /// Check if the process will be daemonized.
      bool
      daemon() const;

      /// Enable/disable daemon mode.
      ProcessConfig& daemon(bool mode);

      /// Get an environment variable of the process configuration.
      std::string const&
      getenv(std::string const& name) const;

      /// Set an environment variable for the process configuration.
      ProcessConfig&
      setenv(std::string const& name,
             std::string const& value);

      /// Set all environment variable from the current process.
      ProcessConfig&
      inherit_current_environment();

      /// Enable redirection of a process input/output channel.
      ProcessConfig&
      create_pipe(ProcessChannelStream const channel);

      /// Connect program output to the input of another program.
      ProcessConfig&
      connect_stdout(ProcessConfig& other);

      /// Pipe a channel from/to a file
      ProcessConfig&
      pipe_file(ProcessChannelStream const channel,
                std::string const& path,
                std::string const& mode = "w");

      /// Check if a channel is enabled.
      bool
      has_pipe(ProcessChannelStream const channel);

      /// Redirect stderr into stdout.
      ProcessConfig& merge_stderr();

    protected:
      // Internal shortcut to retreive a channel.
      ProcessChannel&
      channel(ProcessChannelStream const channel);

      friend class Process;
    };

    /// @brief A successfully launched program.
    class Process
    {
    public:
      /// The status of a child process is determined inspecting it's status
      /// code. Generally, the value 0 means success or still running, while
      /// any other means that something failed.
      typedef int StatusCode;

      /// A unique identifier of a running process.
      typedef int Id;

      typedef std::chrono::milliseconds Milliseconds;

    private:
      struct Impl;
      std::unique_ptr<Impl> _this;
      ProcessConfig         _config;

    public:
      /// Retreive a process configuration.
      ///
      /// @note The configuration is read-only beceause you cannot change
      ///       configuration of a running process.
      ProcessConfig const& config() const { return _config; }

      /// @brief Construct a process (and launch it).
      ///
      /// @param binary     Path to the binary name.
      /// @param arguments  Argument list.
      /// @param model      shortcut for common configurations.
      /// @param config     Custom configuration.
    public:
      Process(ProcessConfig&& config,
              std::string const& binary,
              std::list<std::string> const& arguments = {});

      Process(ProcessConfigModel const model,
              std::string const& binary,
              std::list<std::string> const& arguments = {});

      explicit
      Process(std::string const& binary,
              std::list<std::string> const& arguments = {});

    public:
      /// A process is movable.
      Process(Process&& other);

      /// Kill and wait for the process to terminate unless it is a daemon.
      /// @warning You loose the ability to known whether the child process
      ///          failed or not. Moreover, your process will be violently
      ///          killed. You should always wait() your processes.
      ~Process();

      /// Returns the process exit status, or the negated value of a caught
      /// signal, eventually waiting for its termination.
      /// If term equals `Termination::dont_wait` and the program is still
      /// running, returns 0.
      StatusCode
      status(ProcessTermination const term = ProcessTermination::dont_wait);

      /// Returns true if the program is running.
      bool
      running();

      /// The process Unique identifier.
      Id
      id() const;

      /// @brief Wait for @a timeout milliseconds for the process to terminate.
      ///
      /// If @a timeout equals 0, it will wait indefinitely. If the timeout
      /// is exceeded, it will return 0.
      StatusCode
      wait_status(Milliseconds timeout = Milliseconds(0));

      /// @brief Wait for @a timeout milliseconds for the process to terminate.
      ///
      /// If @a timeout equals 0, it will wait indefinitely. Throws if the
      /// program terminate with error.
      void
      wait(Milliseconds timeout = Milliseconds(0));

      /// Ask the program to interrupt.
      Process&
      interrupt(ProcessTermination const term = ProcessTermination::wait);

      /// Ask the program to terminate.
      Process&
      terminate(ProcessTermination const term = ProcessTermination::wait);

      /// Kill the program immediatly and returns its exit status.
      Process&
      kill(ProcessTermination const term = ProcessTermination::wait);

      /// @brief Read a string from a process' standard output.
      ///
      /// @warning Only valid when the process has its output piped to stdout.
      std::string read(size_t const max = 4096);

    private:
      /// Send a signal to the program.
      void
      _signal(int signal, ProcessTermination const term);
    };

    /// Retrieve a default process config of any model.
    ProcessConfig
    process_config(ProcessConfigModel const model);

    /// @brief Command concept allow operation on processes.
    ///
    /// @a Command stores one or more process configuration to be manipulated
    /// as one process. A command can be fired whenever needed, but only once.
    /// They are compatible with the functions `execute()`, `check_output()`.
    /// They also support pipe operator (`|`) that does what you expect: piping
    /// commands.
    ///
    /// @see @the command() function to create a command.
    template <typename Implem>
    struct ProcessCommandConcept
    {
    protected:
      // This class only describe the command API.
      ProcessCommandConcept();

      // The command concept should not be stored.
      virtual
      ~ProcessCommandConcept();

    public:
      /// Connect stdout to another command input.
      virtual
      Implem&
      connect_stdout(Implem&& other) = 0;

      /// alias for connect_stdout()
      virtual
      Implem&&
      operator |(Implem&& other) = 0;

      /// Execute all programs (but not waited).
      virtual
      Implem&
      execute() = 0;

      /// Wait for all programs to be finished.
      virtual
      Implem&
      wait() = 0;

      /// Wait for all programs to be finished and returns 0 on success or the
      /// status code of the last child process that failed.
      virtual
      Process::StatusCode wait_status() = 0;
    };

    // internal implementation of process command
    namespace detail { struct Command; }

    /// @brief Generate a command.
    ///
    /// Accepted arguments:
    ///  - std::string: binary name or argument
    ///  - std::list<std::string>: some more arguments.
    ///  - ProcessConfig: some configuration.
    ///  - ProcessConfigModel: a model for some configuration.
    ///  - detail::Command: another command.
    ///
    /// Arguments and configuration are merged from left to right
    /// for the created command. A command instance will be appended,
    /// and all following arguments will be applied to it.
    ///
    /// Examples (see the test file for more)
    /// ---------------------------
    /// command("echo", "hello")
    /// command(command("echo"), "hello")
    /// command("echo", "hello", check_output_config)
    /// command("echo", "hello") | command("cat", "-e")
    /// command("echo", "1", command("echo", "2))
    /// command(command("echo", "1"), command("echo", "2))
    template<typename... Args>
    inline
    detail::Command
    command(Args&&... args);

    /// @brief Execute a command and wait its termination.
    ///
    /// @returns 0 on success or the status code of the last child process that
    ///          failed.
    /// @see @the command() function for accepted arguments documentation.
    template <typename... Args>
    Process::StatusCode
    execute(Args&&... args);

    /// @brief Execute a command and returns its output.
    /// @see @the command() function for accepted arguments documentation.
    template <typename... Args>
    std::string
    check_output(Args&&... args);

  }
}

# include "Process.hxx"

#endif
