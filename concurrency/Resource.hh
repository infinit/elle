//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Resource.hh
//
// created       julien quintard   [sun mar 28 00:28:32 2010]
// updated       julien quintard   [sun mar 28 03:15:57 2010]
//

#ifndef ELLE_CONCURRENCY_RESOURCE_HH
#define ELLE_CONCURRENCY_RESOURCE_HH

namespace elle
{
  namespace concurrency
  {

//
// ---------- class -----------------------------------------------------------
//

    ///
    /// this class should be inherited by any class that fibers may
    /// be willing to wait for, such as any concurrency control classes
    /// including Mutex, Semaphore, Condition and so forth.
    ///
    /// basically, a program can be composed of multiple threads, each
    /// handling its own events while each thread can be subdivided into
    /// several fibers.
    ///
    /// therefore, a fiber F belonging to a thread T must, in order to
    /// lock a mutex for instance, acquire the mutex in an atomic way
    /// for concurrency purposes with other threads and then lock the
    /// mutex for the fiber or wait for the resource to be cooperatively
    /// released by the fiber owning the resource for now.
    ///
    /// note that the address of the resource is used as a unique identifier
    /// simply because given an address space, every object has a unique
    /// memory address.
    ///
    class Resource
    {
    };

  }
}

#endif
