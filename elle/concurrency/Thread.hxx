//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Thread.hxx
//
// created       julien quintard   [tue mar  9 18:53:38 2010]
// updated       julien quintard   [sun mar 14 13:03:34 2010]
//

#ifndef ELLE_CONCURRENCY_THREAD_HXX
#define ELLE_CONCURRENCY_THREAD_HXX

namespace elle
{
  namespace concurrency
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the thread in which the given object lives.
    ///
    template <typename T>
    Thread*		Thread::Retrieve(T*			object)
    {
      // dynamic cast the object.
      return ((Thread*)(dynamic_cast< ::QObject* >(object)->thread()));
    }

  }
}

#endif
