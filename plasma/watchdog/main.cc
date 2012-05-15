
#include <cstdio>
#include <iostream>

#include <boost/filesystem.hpp>

#include "Application.hh"
#include "WatchdogInterface.hh"



static void _initAll();

#define BUF_SIZE 4096

int     main(int ac, char* av[])
{
  plasma::watchdog::Application app(ac, av);

  try
    {
      _initAll();
      auto res = app.Exec();
      return res;
    }
  catch (std::exception const& err)
    {
      std::cout << "An exception occured: " << err.what() << std::endl;
    }
  catch (...)
    {
      std::cout << "Uncatched exception\n";
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
        std::cerr << "hangup signal catched\n";
        break;
      case SIGTERM:
        std::cerr << "terminate signal catched\n";
        break;
      }
}

/**
 * create background process out of the application, source code taken from:
 * http://www.enderunix.org/docs/eng/daemon.php with some minor modifications
 */
static void _init_daemon(std::string const& infinit_home)
{
    namespace fs = boost::filesystem;
    int i,lfp;
    char str[10];
    if(getppid()==1)
        return; /* already a daemon */
    i=fork();
    if (i<0)
        exit(1); /* fork error */
    if (i>0)
        exit(0); /* parent exits */

    /* child (daemon) continues */
    setsid(); /* obtain a new process group */

    for (i=getdtablesize();i>=0;--i)
        close(i); /* close all descriptors */
    i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */

    umask(027); /* set newly created file permissions */

    chdir(infinit_home.c_str()); /* change running directory */
    auto lock_file = fs::path(infinit_home).append("lock.wtg", fs::path::codecvt()).string();
    lfp = open(lock_file.c_str(), O_RDWR | O_CREAT, 0640);
    if (lfp < 0)
        exit(1); /* can not open */
    if (lockf(lfp, F_TLOCK,0) < 0)
        exit(0); /* can not lock */
    /* first instance continues */
    sprintf(str,"%d\n",getpid());
    write(lfp, str, strlen(str)); /* record pid to lockfile */
    signal(SIGCHLD, SIG_IGN); /* ignore child */
    signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGHUP,  &_signal_handler); /* catch hangup signal */
    signal(SIGTERM, &_signal_handler); /* catch kill signal */
}

#include "elle/Elle.hh"
#include "lune/Lune.hh"
#include "nucleus/Nucleus.hh"

static void _initAll()
{

  if (elle::Elle::Initialize() == elle::StatusError ||
      lune::Lune::Initialize() == elle::StatusError ||
      nucleus::Nucleus::Initialize() == elle::StatusError)
    throw std::runtime_error("Couldn't initialize !");

  // XXX use elle here

  _init_daemon(lune::Lune::Home.string);
}
