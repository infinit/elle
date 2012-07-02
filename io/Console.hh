#ifndef ELLE_IO_CONSOLE_HH
# define ELLE_IO_CONSOLE_HH

# include <elle/types.hh>

namespace elle
{
  namespace io
  {

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
