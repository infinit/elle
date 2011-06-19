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
// updated       julien quintard   [sat jun 18 20:47:23 2011]
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
    /// note that the address of the resource is used as a unique identifier
    /// simply because given an address space, every object has a unique
    /// memory address.
    ///
    /// note that both Object and Entity classes inherit this class.
    ///
    class Resource
    {
    };

  }
}

#endif
