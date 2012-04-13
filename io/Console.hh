//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat jun 18 11:09:29 2011]
//

#ifndef ELLE_IO_CONSOLE_HH
#define ELLE_IO_CONSOLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <elle/radix/Status.hh>

namespace elle
{

  using namespace radix;

  namespace io
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this static class provides functionalities related to the console.
    ///
    class Console
    {
    public:
      //
      // enumerations
      //
      enum Option
        {
          OptionNone,

          OptionPassword
        };

      //
      // static methods
      //
      static Status     Input(String&,
                              const String& = "",
                              const Option = OptionNone);
    };

  }
}

#endif
