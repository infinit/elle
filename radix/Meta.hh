//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun nov 29 19:31:55 2009]
//

#ifndef ELLE_RADIX_META_HH
#define ELLE_RADIX_META_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Void.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Boolean.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Trace.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <stdlib.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;

  namespace radix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the root of the hierarchy. every class
    /// should directly or indirectly derive this class.
    ///
    /// note that the whole allocation process can be traced by
    /// activating the Debug::Status variable.
    class Meta
    {
    public:
      //
      // constants
      //
      struct                            Debug
      {
        static const Boolean            Status;
        static Boolean                  State;
      };

      //
      // static methods
      //
      static Status     Initialize();
      static Status     Clean();

      static Status     Enable();
      static Status     Disable();

      static Status     Show(const Natural32 = 0);

      //
      // constructors & destructors
      //
      virtual ~Meta();

      //
      // operators
      //
      void*             operator new(size_t);
      void*             operator new(size_t,
                                     void*);

      void*             operator new[](size_t);
      void*             operator new[](size_t,
                                       void*);

      void              operator delete(void*);
    };

  }
}

#endif
