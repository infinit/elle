#include <elle/io/Console.hh>

#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <stdio.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace io
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method retrieves the user's input from the console.
    ///
    Status              Console::Input(String&                  text,
                                       const String&            prompt,
                                       const Console::Option    option)
    {
      // retrieve the input depending on the option.
      switch (option)
        {
        case OptionPassword:
          {
            // retrieve the input and assign it.
            text.assign(::readline(prompt.c_str()));

            break;
          }
        case OptionNone:
        default:
          {
            // retrieve the input and assign it.
            text.assign(::readline(prompt.c_str()));

            break;
          }
        }

      return Status::Ok;
    }

  }
}
