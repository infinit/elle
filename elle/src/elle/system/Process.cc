#include <elle/system/Process.hh>

#include <sys/types.h>
#include <sys/wait.h>

#include <cerrno>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.system.Process");

namespace elle
{
  namespace system
  {
    class Process::Impl
    {
    public:
      Impl(Process& owner)
        : _owner(owner)
        , _pid(0)
        , _status(0)
        , _done(false)
      {
        this->_pid = fork();
        if (this->_pid == 0)
        {
          auto const& args = this->_owner.arguments();
          std::unique_ptr<char const*[]> argv(new char const*[args.size() + 1]);
          int i = 0;
          for (auto const& arg: args)
            argv[i++] = arg.c_str();
          argv[i] = nullptr;
          execvp(argv[0], const_cast<char**>(argv.get()));
        }
      }

      void
      wait()
      {
        pid_t waited;
        do
        {
          waited = ::waitpid(this->_pid, &this->_status, 0);
        }
        while (waited == -1 && errno == EINTR);
        if (waited == -1)
          throw elle::Exception(elle::sprintf("unable to wait process: %s",
                                              strerror(errno)));
        assert(waited == this->_pid);
        this->_done = true;
      }

    private:
      ELLE_ATTRIBUTE(Process&, owner);
      ELLE_ATTRIBUTE_R(pid_t, pid);
      ELLE_ATTRIBUTE_R(int, status);
      ELLE_ATTRIBUTE_R(bool, done);
    };

    Process::Process(std::vector<std::string> args)
      : _arguments(std::move(args))
      , _impl(new Process::Impl(*this))
    {
      ELLE_TRACE_SCOPE("%s: start");
    }

    int
    Process::wait()
    {
      if (!this->_impl->done())
        this->_impl->wait();
      assert(this->_impl->done());
      return this->_impl->status();
    }

    Process::~Process()
    {}
  }
}
