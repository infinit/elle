#ifndef ELLE_RADIX_META_HH
# define ELLE_RADIX_META_HH

#include <elle/types.hh>

#include <elle/radix/Trace.hh>

namespace elle
{
  namespace radix
  {

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
