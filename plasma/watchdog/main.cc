
#include "Application.hh"

#include <elle/log.hh>
#include <elle/os/getenv.hh>
#include <elle/CrashReporter.hh>

#include <common/common.hh>

#include <cstdio>
#include <fstream>
#include <iostream>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

ELLE_LOG_COMPONENT("infinit.plasma.watchdog");

static void _initAll(std::string const& user_id);

// Signals handler must have access to the application.
static plasma::watchdog::Application *app = nullptr;

int     main(int ac, char* av[])
{

  if (ac != 2 || av[1] == nullptr)
    {
      std::cerr << "usage: " << av[0] << " user_id" << std::endl;
      return EXIT_FAILURE;
    }

  std::string user_id{av[1]};

  app = new plasma::watchdog::Application{user_id};

  try
    {
      _initAll(user_id);
      ELLE_DEBUG("Starting the watchdog for user %s", user_id);

      ::setenv("ELLE_LOG_COMPONENTS", "infinit.*,reactor.network.*", 1);
      ::setenv("ELLE_LOG_LEVEL", "DEBUG", 1);

      std::string log_file = common::watchdog::log_path(user_id);
      if (freopen(log_file.c_str(), "a+", stderr) == NULL ||
          freopen(log_file.c_str(), "a+", stdout) == NULL)
        fprintf(stderr, "Cannot open %s: %s", log_file.c_str(), strerror(errno));

      auto res = app->exec();
      return res;
    }
  catch (std::exception const& err)
    {
      elle::crash::report("8watchdog", err.what());
      ELLE_ERR("An exception occured: %s", err.what());
    }
  catch (...)
    {
      elle::crash::report("8watchdog");
      ELLE_ERR("Uncaught exception");
    }
  return EXIT_FAILURE;
}

///////////////////////////////////////////////////////////////////////////////
// Initialization code below

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

/**
 * a signal handler for the Linux signals sent to daemon process, for more
 * signals, refer to
 * http://www.comptechdoc.org/os/linux/programming/linux_pgsignals.html
 */


static void _signal_handler(int sig)
{
    switch(sig)
      {
      case SIGHUP:
        std::cerr << "caught SIGHUP\n";
        break;
      case SIGTERM:
        std::cerr << "caught SIGTERM\n";
        break;
      case SIGCHLD:
        std::cerr << "caught SIGCHLD\n";
        break;
      case SIGKILL:
        std::cerr << "caught SIGKILL\n";
        break;
      default:
        std::cerr << "Unkwown signal caught\n";
      }
}

/**
 * create background process out of the application, source code taken from:
 * http://www.enderunix.org/docs/eng/daemon.php with some minor modifications
 */
static
void
_init_daemon(std::string const& user_id)
{
    auto lock_file = common::watchdog::lock_path(user_id);
    int lfp;
    char str[20]; // used to store the pid.
    if (::getppid() != 1) // if parent pid
      {
        // first fork
          {
            int res = fork();
            if (res < 0)
              {
                perror("fork");
                exit(1); /* fork error */
              }
            if (res > 0)
              {
                exit(0); /* parent exits */
              }
          }


        /* child (daemon) continues */
        setsid(); /* obtain a new process group */

        // double fork
          {
            int res = ::fork();
            if (res < 0)
              {
                perror("fork");
                exit(1); /* fork error */
              }
            if (res > 0)
              {
                exit(0); /* parent exits */
              }
          }


          if (::getenv("WATCHDOG_STARTUP_SLEEP") != nullptr)
          {
            fprintf(stderr, "===== WATCHDOG_STARTUP_SLEEP. Pid: %i  =====\n",
                    getpid());
            ::sleep(10);
            fprintf(stderr, "===== FINISHED =====\n");
          }
        //for (i=getdtablesize();i>=0;--i)
        //  close(i); /* close all descriptors */
        //i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */

        umask(027); /* set newly created file permissions */

        ::chdir("/");

        lfp = ::open(lock_file.c_str(), O_RDWR | O_CREAT, 0640);
        if (lfp < 0)
          {
            ELLE_ERR("Can not open '%s'.", lock_file.c_str());
            exit(1); /* can not open */
          }
        if (lockf(lfp, F_TLOCK,0) < 0)
          {
            ELLE_ERR("Can not lock '%s'.", lock_file.c_str());
            exit(0); /* can not lock */
          }
        /* first instance continues */
        sprintf(str,"%d\n", getpid());
        write(lfp, str, strlen(str)); /* record pid to lockfile */
      }
    else
      ELLE_WARN("Already a deamon!");

#define SIG_CONNECT(sig)                                                      \
      {                                                                       \
        struct sigaction sa;                                                  \
        sa.sa_handler = &_signal_handler;                                     \
        sigemptyset(&sa.sa_mask);                                             \
        sa.sa_flags = 0;                                                      \
        sa.sa_flags |= SA_RESTART;                                            \
        if (sigaction(sig, &sa, 0) > 0)                                       \
          std::cerr << "Couldn't connect to " #sig "\n";                      \
      }                                                                       \

    //SIG_CONNECT(SIGCHLD);
    SIG_CONNECT(SIGHUP);
    SIG_CONNECT(SIGTERM);
    SIG_CONNECT(SIGKILL);
    SIG_CONNECT(SIGQUIT);
    SIG_CONNECT(SIGINT);
#undef SIG_CONNECT

    signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
}

#include <elle/types.hh>
#include "lune/Lune.hh"

static void _initAll(std::string const& user_id)
{
  if (lune::Lune::Initialize() == elle::Status::Error)
    throw std::runtime_error("Couldn't initialize !");

  _init_daemon(user_id);
}
