#include <elle/system/Process.hh>

#ifndef INFINIT_WINDOWS
# include <sys/types.h>
# include <sys/wait.h>
#else
# include <elle/windows.h>
#endif

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
#ifndef INFINIT_WINDOWS
        , _pid(0)
#else
        , _process_info()
#endif
        , _status(0)
        , _done(false)
      {
#ifndef INFINIT_WINDOWS
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
          ELLE_ERR("execvp error: %s", strerror(errno));
          ::exit(1);
        }
#else
        STARTUPINFO startup_info = {sizeof(STARTUPINFO)};
        std::string executable = this->_owner.arguments()[0];
        std::string command_line;
        int i = 0;
        for (auto const& arg: this->_owner.arguments())
        {
          if (i++ == 0)
            continue;
          if (i > 1)
            command_line += " ";
          command_line += arg;
        }
        if (!::CreateProcess(executable.c_str(), strdup(command_line.c_str()),
                             NULL, NULL, true, 0, NULL, NULL,
                             &startup_info, &this->_process_info))
        {
          throw elle::Exception(elle::sprintf("unable to start %s: %s",
                                              executable, ::GetLastError()));
        }
#endif
      }

      void
      wait()
      {
#ifndef INFINIT_WINDOWS
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
#else
        ::WaitForSingleObject(this->_process_info.hProcess, INFINITE);
        unsigned long status = 0;
        ::GetExitCodeProcess(this->_process_info.hProcess, &status);
        this->_status = status;
#endif
        this->_done = true;
      }

    private:
      ELLE_ATTRIBUTE(Process&, owner);
#ifndef INFINIT_WINDOWS
      ELLE_ATTRIBUTE_R(pid_t, pid);
#else
      ELLE_ATTRIBUTE_R(PROCESS_INFORMATION, process_info)
#endif
      ELLE_ATTRIBUTE_R(int, status);
      ELLE_ATTRIBUTE_R(bool, done);
    };

    Process::Process(std::vector<std::string> args)
      : _arguments(std::move(args))
      , _impl(new Process::Impl(*this))
    {
      ELLE_TRACE_SCOPE("%s: start", *this);
    }

    int
    Process::wait()
    {
      if (!this->_impl->done())
        this->_impl->wait();
      ELLE_ASSERT(this->_impl->done());
      return this->_impl->status();
    }
     int
    Process::pid()
    {
#ifndef INFINIT_WINDOWS
      return this->_impl->pid();
#else
      return this->_impl->process_info().dwProcessId;
#endif
    }
    Process::~Process()
    {}
  }
}
