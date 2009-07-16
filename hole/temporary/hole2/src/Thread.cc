//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Jan 25 07:35:29 2009 texane
// Last update Sun Jan 25 08:40:30 2009 texane
//



#ifdef _WIN32_WINNT
# include <windows.h>
#else // posix
# include <pthread.h>
#endif

#include "Thread.hh"
#include "Debug.hh"



#ifdef _WIN32_WINNT
static DWORD __stdcall threadEntry(LPVOID p)
#else // posix
static void* threadEntry(void* p)
#endif
{
  Thread* thread = static_cast<Thread*>(p);

  thread->entry();

#ifdef _WIN32_WINNT
  return 0;
#else // posix
  return NULL;
#endif

}


Thread::Thread()
{
#ifdef _WIN32_WINNT

  thread_ = NULL;

#else // posix

  isThreadValid_ = false;

#endif
}


bool Thread::start()
{
#ifdef _WIN32_WINNT

  thread_ = ::CreateThread(NULL, 0, threadEntry, this, 0, NULL);
  if (thread_ == NULL)
    {
      DEBUG_ERROR("CreateThread() == %u\n", GetLastError());
      return false;
    }

  return true;

#else // posix

  DEBUG_ASSERT(isThreadValid_ == false);

  if (::pthread_create(&thread_, NULL, threadEntry, this))
    {
      DEBUG_ERROR("pthread_create()\n");
      return false;
    }

  isThreadValid_ = true;

  return true;

#endif
}


void Thread::join()
{
#ifdef _WIN32_WINNT

  if (thread_ != NULL)
    {
      ::WaitForSingleObject(thread_, INFINITE);
      ::CloseHandle(thread_);
      thread_ = NULL;
    }

#else // posix

  if (isThreadValid_)
    {
      void* p;
      ::pthread_join(thread_, &p);
      isThreadValid_ = false;
    }

#endif
}



#if 0 // unit


#include <cstdio>
#include <vector>


using std::vector;


class PrinterThread : public Thread
{
private:

  int i_;

public:

  PrinterThread(int i) :
    i_(i) {}

  ~PrinterThread()
  {
    ::printf("%s\n", __FUNCTION__);
  }

  void entry()
  {
    ::printf("[%02x]: foo\n", i_);
  }

};


int main(int, char**)
{
  int i = 0;

#define THREAD_COUNT 10
  vector<Thread*> threads(THREAD_COUNT);

  vector<Thread*>::iterator end = threads.end();

  for (vector<Thread*>::iterator pos = threads.begin(); pos != end; ++pos)
    *pos = new PrinterThread(i++);

  for (vector<Thread*>::iterator pos = threads.begin(); pos != end; ++pos)
    {
      if ((*pos)->start() == false)
	{
	  delete *pos;
	  *pos = NULL;
	}
    }

  for (vector<Thread*>::iterator pos = threads.begin(); pos != end; ++pos)
    {
      Thread* thread = *pos;

      if (thread != NULL)
	{
	  thread->join();
	  delete thread;
	}
    }

  return 0;
}


#endif
