//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Jan 25 06:25:30 2009 texane
// Last update Sun Jan 25 07:01:41 2009 texane
//



#ifndef SYNC_EVENT_HH_INCLUDED
# define SYNC_EVENT_HH_INCLUDED



#ifdef _WIN32_WINNT
# include <windows.h>
#else /* posix */
# include <pthread.h>
#endif



class SyncEvent
{
private:

#if _WIN32_WINNT

  HANDLE eventHandle_;

#else // posix

  bool isValid_;

  pthread_cond_t cond_;
  pthread_mutex_t mutex_;

  volatile bool isSet_;

#endif // _WIN32_WINNT

public:

  SyncEvent();
  ~SyncEvent();

  bool set();
  bool wait();

  bool isValid() const;
};



#endif // ! SYNC_EVENT_HH_INCLUDED
