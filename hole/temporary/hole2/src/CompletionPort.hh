//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Jan 25 09:21:16 2009 texane
// Last update Sun Jan 25 10:53:03 2009 texane
//



#ifndef COMPLETION_PORT_HH_INCLUDED
# define COMPLETION_PORT_HH_INCLUDED



#include <vector>
#include "Atomic.hh"
#include "SyncEvent.hh"



// a completion pool is made of completable
// objects where only the last one set the
// completion event.


class CompletionPort
{
private:

  SyncEvent syncEvent_;
  AtomicIntegral counter_;

public:

  CompletionPort(int counter) :
    counter_(counter + 1) { }

  void complete();
  bool wait();

};



#endif // COMPLETION_POOL_HH_INCLUDED
