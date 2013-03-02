#ifndef  ELLE_SYSTEM_PROCESS_HH
# define ELLE_SYSTEM_PROCESS_HH

# include <iosfwd>
# include <list>
# include <memory>
# include <string>

namespace elle
{
  namespace system
  {

    /// Shortcuts for common process configurations.
    enum ProcessKind
    {
      /// Environment is empty.
      empty_config = 0,

      /// All environment variables inherited.
      normal_config = 1 << 1,

      /// Environment inherited + stdout piped.
      check_output_config = normal_config | 1 << 2,

      /// Detach from parent.
      daemon_config = 1 << 2,
    };


    /// Flags for process actions.
    enum class ProcessTermination { wait, dont_wait, };

    /// Input and output streams for a process.
    enum class ProcessChannel { in, out, err };

    struct Channel;

    /// Customize a process launch.
    class ProcessConfig
    {
    private:
      struct Impl;
      std::unique_ptr<Impl> _impl;

    public:
      ProcessConfig();
      ProcessConfig(ProcessConfig&& other);
      ~ProcessConfig();

      bool daemon() const;
      ProcessConfig& daemon(bool mode);

      std::string const&
      getenv(std::string const& name) const;

      ProcessConfig&
      setenv(std::string const& name,
             std::string const& value);

      ProcessConfig&
      inherit_current_environment();

      ProcessConfig&
      create_pipe(ProcessChannel const channel);

      bool has_pipe(ProcessChannel const channel);

      ProcessConfig& merge_stderr();
    protected:
      Channel& channel(ProcessChannel const channel);

      friend class Process;

    };

    class Process
    {
    public:
      typedef int StatusCode;

    private:
      struct Impl;
      std::unique_ptr<Impl> _impl;
      ProcessConfig         _config;

    public:
      ProcessConfig const& config() const { return _config; }

      /// @brief Construct a process (and launch it).
      ///
      /// @param binary     Path to the binary name.
      /// @param arguments  Argument list.
      /// @param kind       shortcut for common configurations.
      /// @param config     Custom configuration.
    public:
      Process(ProcessConfig&& config,
              std::string const& binary,
              std::list<std::string> const& arguments);

      Process(ProcessKind const kind,
              std::string const& binary,
              std::list<std::string> const& arguments);

      Process(std::string const& binary,
              std::list<std::string> const& arguments);

      explicit
      Process(std::string const& binary);



    public:
      Process(Process&& other);

      /// Kill and wait for the process to terminate unless it is a daemon.
      ~Process();

      /// Returns the process exit status, or the negated value of a caught
      /// signal, eventually waiting for its termination.
      /// If term equals `Termination::dont_wait` and the program is still
      /// running, returns 0.
      StatusCode
      status(ProcessTermination const term = ProcessTermination::dont_wait);

      /// Returns true if the program is running.
      bool running();

      /// Alias for status(Termination::wait)
      StatusCode wait_status();

      /// Wait until the process finish and throw in case of error.
      void wait();

      /// Kill the program immediatly and returns its exit status.
      void kill(ProcessTermination const term = ProcessTermination::wait);

      /// Ask the program to terminate.
      void terminate(ProcessTermination const term = ProcessTermination::wait);

      std::string read(size_t const max = 4096);
    };

    /// Retrieve a default process config of any kind.
    ProcessConfig
    process_config(ProcessKind const kind);

    /// @brief shortcut to create a process.
    ///
    /// Accepts the same arguments as the Process constructor, but can also
    /// accept process arguments as a strings instead of a separate list.
    ///
    template <typename... Args>
    Process execute(Args&&... args);

    /// @brief Execute a process and returns its output.
    template <typename... Args>
    std::string check_output(Args&&... args);

  }
}

# include "Process.hxx"

#endif
