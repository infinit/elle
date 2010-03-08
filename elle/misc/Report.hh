//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/misc/Report.hh
//
// created       julien quintard   [sun oct 28 19:12:38 2007]
// updated       julien quintard   [mon mar  8 23:07:17 2010]
//

#ifndef ELLE_MISC_REPORT_HH
#define ELLE_MISC_REPORT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>
#include <elle/core/Void.hh>

///
/// undefine the conflicting macro
///
#undef fail
#include <sstream>
#include <stack>
#include <ostream>
#include <iostream>

#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include <elle/core/Macro.hh>

namespace elle
{
  using namespace core;

  namespace misc
  {

//
// ---------- class -----------------------------------------------------------
//

    ///
    /// this classes represents a log report.
    ///
    /// note that the elle library never flushes or print the report messages.
    /// the application using the library should therefore either flush or
    /// print the messages when the faulting call returns before performing
    /// any other call the the library. otherwise, the error messages will
    /// be mixed.
    ///
    /// \todo add a date to the messages and remove the indentation
    ///
    class Report
    {
    public:
      //
      // enumerations
      //

      ///
      /// errors are commonly reported when something wrong occurs being
      /// in a normal method/function or in a function/method returns
      /// a boolean.
      ///
      /// failures are critical errors. every failure notification is
      /// followed by the program stopping.
      ///
      /// warning are additional messages than can be issued by
      /// methods/functions.
      ///
      enum Type
	{
	  TypeWarning,
	  TypeError,
	  TypeFailure
	};

      //
      // structures
      //
      struct Entry
      {
	Type		type;
	String*		message;
      };

      //
      // constructors & destructors
      //
      ~Report();

      //
      // methods
      //
      Void		Flush();
      Void		Notify(Type,
			       const String&);

      //
      // attributes
      //
      std::stack<Entry*>	store;
    };

//
// ---------- externs ---------------------------------------------------------
//

    extern Report	report;

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  std::ostream&		operator<<(std::ostream&,
				   elle::misc::Report&);

}

#endif
