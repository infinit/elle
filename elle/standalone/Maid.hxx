//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun feb 28 09:08:48 2010]
//

#ifndef ELLE_STANDALONE_MAID_HXX
#define ELLE_STANDALONE_MAID_HXX

namespace elle
{
  namespace standalone
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns a guard instance that will keep track
    /// of objects allocated through the new operator.
    ///
    /// note that the object is created in the area pointed by the
    /// memory variable i.e within the caller's stack frame.
    ///
    template <typename T>
    inline Maid::Guard*		Maid::Monitor(Void*		memory,
					      T&		pointer)
    {
      // first initialize the pointer to null.
      pointer = NULL;

      // then, return a Guard to keep track of the given pointer.
      return (new (memory) Maid::Instance<T>(pointer));
    }

    ///
    /// this method returns a guard instance that will keep track
    /// of objects allocated through the use of a speific function
    /// such as RSA_new(), malloc() or even BN_init()
    ///
    /// note that the object is created in the area pointed by the
    /// memory variable i.e within the caller's stack frame.
    ///
    template <typename T, typename F>
    inline Maid::Guard*		Maid::Monitor(Void*		memory,
					      T&		pointer,
					      F			function)
    {
      // first initialize the pointer to null.
      pointer = NULL;

      // then return a Guard to keep track of the given pointer.
      return (new (memory) Maid::Slab<T, F>(pointer, function));
    }

    ///
    /// this method creates a Garrison by storing the set of given
    /// and freshly built guards.
    ///
    /// note that the object is created in the area pointed by the
    /// memory variable i.e within the caller's stack frame.
    ///
    template <typename... T>
    inline Maid::Garrison*	Maid::Install(Void*		memory,
					      T...		guards)
    {
      // create the Garrison instance.
      return (new (memory) Maid::Garrison(sizeof... (T), guards...));
    }

    ///
    /// this specialization is required to avoid creating a garrison
    /// should no objects be provided which is what happens most of
    /// the time.
    ///
    template <typename... T>
    inline Maid::Garrison*	Maid::Install(Void*)
    {
      return (NULL);
    }

  }
}

#endif
