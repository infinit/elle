//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [sat mar 24 18:06:40 2012]
//

//
// ---------- includes --------------------------------------------------------
//

#include <horizon/macosx/Event.hh>

using namespace horizon::macosx;

//
// ---------- constructors & destructors --------------------------------------
//

Event::Event(boost::interprocess::interprocess_semaphore*       semaphore,
             int*                                               result):
  QEvent(::QEvent::User),

  operation(OperationUnknown),
  semaphore(semaphore),
  result(result)
{
}
