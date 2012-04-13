//
// ---------- header ----------------------------------------------------------
//
// project       shell
//
// license       infinit
//
// author        julien quintard   [sat mar 27 08:37:14 2010]
//

#ifndef SHELL_SHELL_HH
#define SHELL_SHELL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>

#include <elle/idiom/Close.hh>
# include <readline/readline.h>
# include <readline/history.h>
#include <elle/idiom/Open.hh>

namespace satellite
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the shell satellite.
  ///
  class Shell
  {
  public:
    //
    // types
    //
    typedef struct
    {
      const elle::Character*    name;
      elle::Status              (*function)(const elle::String&);
    }                           Command;

    //
    // methods
    //
    static elle::Status         Help(const elle::String&);
    static elle::Status         Quit(const elle::String&);
    static elle::Status         Dump(const elle::String&);

    //
    // attributes
    //
    static nucleus::Address     Address;
  };

}

#endif
