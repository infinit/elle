//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Jan 25 06:19:25 2009 texane
// Last update Sun Jan 25 10:11:16 2009 texane
//



#ifdef _WIN32_WINNT
# include <windows.h>
#else /* posix */
# include <pthread.h>
#endif

#include <cstdlib>

#include "Debug.hh"
#include "SyncEvent.hh"



SyncEvent::SyncEvent()
{
#ifdef _WIN32_WINNT

  eventHandle_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
  if (eventHandle_ == NULL)
    DEBUG_ERROR("CreateEvent() == %u\n", GetLastError());

#else // posix

  isValid_ = false;

  if (!::pthread_cond_init(&cond_, NULL))
    {
      if (!::pthread_mutex_init(&mutex_, NULL))
	{
	  isSet_ = false;
	  isValid_ = true;
	  return ;
	}

      ::pthread_cond_destroy(&cond_);
    }

#endif // _WIN32_WINNT
}


SyncEvent::~SyncEvent()
{
  // assume safe context

#ifdef _WIN32_WINNT

  if (isValid())
    {
      ::CloseHandle(eventHandle_);
      eventHandle_ = NULL;
    }

#else // posix

  if (isValid())
    {
      ::pthread_cond_destroy(&cond_);
      ::pthread_mutex_destroy(&mutex_);
      isValid_ = false;
    }

#endif // _WIN32_WINNT
}


bool SyncEvent::isValid() const
{
#ifdef _WIN32_WINNT

  return eventHandle_ == NULL;

#else // posix

  return isValid_;

#endif // _WIN32_WINNT
}


bool SyncEvent::set()
{
  DEBUG_ASSERT(isValid());

#ifdef _WIN32_WINNT

  if (::SetEvent(eventHandle_) == FALSE)
    {
      DEBUG_ERROR("SetEvent() == %u\n", ::GetLastError());
      return false;
    }

  return true;

#else // posix

  bool isSuccess = false;

  if (::pthread_mutex_lock(&mutex_))
    return false;

  if (!::pthread_cond_signal(&cond_))
    {
      isSet_ = true;
      isSuccess = true;
    }

  ::pthread_mutex_unlock(&mutex_);

  return isSuccess;

#endif // _WIN32_WINNT
}


bool SyncEvent::wait()
{
  DEBUG_ASSERT(isValid());

#ifdef _WIN32_WINNT

  if (::WaitForSingleObject(eventHandle_, INFINITE) != WAIT_OBJECT_0)
    {
      DEBUG_ERROR("WaitForSingleObject() == %u\n", ::GetLastError());
      return false;
    }

  // auto reseted

  return true;

#else // posix

  bool isSuccess = true;

  if (::pthread_mutex_lock(&mutex_))
    {
      DEBUG_ERROR("pthread_mutex_lock()\n");
      return false;
    }

  while (isSuccess && (isSet_ == false))
    if (::pthread_cond_wait(&cond_, &mutex_))
      {
	DEBUG_ERROR("pthread_cond_wait()\n");
	isSuccess = false;
      }

  /* reset event
   */

  isSet_ = false;
  
  ::pthread_mutex_unlock(&mutex_);

  return isSuccess;

#endif // _WIN32_WINNT
}



#if 0 // unit testing


#include <string>

#include <cstdio>
#include "Thread.hh"


using std::string;


class PingerThread : public Thread
{
private:

  SyncEvent myEvent_;
  SyncEvent* hisEvent_;
  bool isPing_;

public:

  PingerThread(bool isPing) :
    hisEvent_(NULL), isPing_(isPing)
  {
  }

  SyncEvent* getMyEvent()
  {
    return &myEvent_;
  }

  void setHisEvent(SyncEvent* hisEvent)
  {
    hisEvent_ = hisEvent;
  }

  // implement Thread

  void entry()
  {
    unsigned int count = 10;

    while (count--)
      {
	if (isPing_)
	  {
	    ::printf("[ping] >\n");
	    myEvent_.wait();
	    ::printf("[ping] <\n");
	    hisEvent_->set();
	  }
	else
	  {
	    hisEvent_->set();
	    ::printf("[pong] >\n");
	    myEvent_.wait();
	    ::printf("[pong] <\n");
	  }
      }
  }

};


int main(int, char**)
{
  PingerThread ping(true);
  PingerThread pong(false);

  ping.setHisEvent(pong.getMyEvent());
  pong.setHisEvent(ping.getMyEvent());

  ping.start();
  pong.start();

  static_cast<Thread*>(&ping)->join();
  static_cast<Thread*>(&pong)->join();

  return 0;
}

#endif
