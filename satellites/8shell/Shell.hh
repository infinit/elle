//
// ---------- header ----------------------------------------------------------
//
// project       8debug
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8shell/Shell.hh
//
// created       julien quintard   [sat mar 27 08:37:14 2010]
// updated       julien quintard   [wed jul  6 14:02:33 2011]
//

#ifndef DEBUG_DEBUG_HH
#define DEBUG_DEBUG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <elle/idiom/Close.hh>
# include <readline/readline.h>
# include <readline/history.h>
#include <elle/idiom/Open.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the 8debug application.
  ///
  class Shell
  {
  public:
    //
    // types
    //
    typedef struct
    {
      const elle::Character*	name;
      elle::Status		(*function)(const elle::String&);
    }				Command;

    //
    // methods
    //
    static elle::Status		Dump(const elle::String&);
    static elle::Status		Help(const elle::String&);
    static elle::Status		Quit(const elle::String&);

    //
    // attributes
    //
    static nucleus::Address	Address;
  };

}

#endif
