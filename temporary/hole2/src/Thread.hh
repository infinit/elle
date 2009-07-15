//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Jan 25 07:08:35 2009 texane
// Last update Sun Jan 25 08:39:43 2009 texane
//



#ifndef THREAD_HH_INCLUDED
# define THREAD_HH_INCLUDED



#ifdef _WIN32_WINNT
# include <windows.h>
#else // posix
# include <pthread.h>
#endif



class Thread
{

private:

#ifdef _WIN32_WINNT
  HANDLE thread_;
#else // posix
  pthread_t thread_;
  bool isThreadValid_;
#endif

public:

  Thread();

  virtual ~Thread() {}

  virtual void entry() = 0;

  bool start();
  void join();
};



#endif // ! THREAD_HH_INCLUDED
