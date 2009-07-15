//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Jan 25 09:20:10 2009 texane
// Last update Sun Jan 25 11:06:32 2009 texane
//



#include <cstdio>
#include "CompletionPort.hh"



void CompletionPort::complete()
{
  if (--counter_)
    return ;

  // set if last one

  syncEvent_.set();
}


bool CompletionPort::wait()
{
  if (!--counter_)
    return true;

  return syncEvent_.wait();
}



#if 0 // unit


#include <vector>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>

#include "Thread.hh"


using std::vector;


class WorkerThread : public Thread
{
private:

  int i_;
  CompletionPort* complPort_;

public:

  WorkerThread(CompletionPort* complPort, int i)
  {
    complPort_ = complPort;
    i_ = i;
  }

  void entry()
  {
    ::printf("[%u] >\n", i_);
    ::usleep((::rand() % 100000));
    ::printf("[%u] <\n", i_);

    complPort_->complete();
  }
};


int main(int, char**)
{
#define THREAD_COUNT 100
  vector<Thread*> threads(THREAD_COUNT);
  vector<Thread*>::iterator end = threads.end();

  CompletionPort complPort(THREAD_COUNT);

  srand(getpid() * time(NULL));

  for (vector<Thread*>::iterator pos = threads.begin(); pos != end; ++pos)
    {
      static int i = 0;
      *pos = new WorkerThread(&complPort, i++);
    }

  for (vector<Thread*>::iterator pos = threads.begin(); pos != end; ++pos)
    {
      Thread* thread = *pos;

      if (thread->start() == false)
	complPort.complete();
    }

  ::printf("> complPort.wait()\n");
  complPort.wait();
  ::printf("< complPort.wait()\n");

  for (vector<Thread*>::iterator pos = threads.begin(); pos != end; ++pos)
    delete *pos;

  return 0;
}

#endif
