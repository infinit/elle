#ifndef  ELLE_SYSTEM_PROCESS_HH
# define ELLE_SYSTEM_PROCESS_HH

# include <list>
# include <memory>
# include <string>

namespace elle
{
  namespace system
  {

    class Process
    {
    public:
      enum class Kind
      {
        normal,
        daemon,
      };

      enum class Termination
      {
        wait,
        dont_wait,
      };

    private:
      struct Impl;
      std::unique_ptr<Impl> _impl;

    public:
      Process(Kind const kind,
              std::string const& binary,
              std::list<std::string> const& arguments);

      /// Construct a process (with Kind::normal)
      Process(std::string const& binary,
              std::list<std::string> const& arguments);

      Process(Process&& other);

      /// Wait for the process to terminate unless it is a daemon.
      ~Process();

      /// Returns the process exit status, or the negated value of a caught
      /// signal, eventually waiting for its termination.
      /// If term equals `Termination::dont_wait` and the program is still
      /// running, returns 0.
      int status(Termination const term = Termination::dont_wait);

      /// Returns true if the program is running.
      bool running();

      /// Alias for status(Termination::wait)
      int wait_status();

      /// Wait until the process finish and throw in case of error.
      void wait();

      /// Kill the program immediatly and returns its exit status.
      void kill(Termination const term = Termination::wait);

      /// Ask the program to terminate.
      void terminate(Termination const term = Termination::wait);
    };

  }
}

#endif
