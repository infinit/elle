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
// updated       julien quintard   [sun aug  2 22:59:38 2009]
//

#ifndef ELLE_MISC_REPORT_HH
#define ELLE_MISC_REPORT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <sstream>
#include <stack>
#include <ostream>
#include <iostream>

#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

namespace elle
{
  using namespace core;

  namespace misc
  {

//
// ---------- macro functions -------------------------------------------------
//

///
/// this macro-function register a report entry.
///
#define report(_type_, _text_)						\
  do									\
    {									\
      std::ostringstream	_message_;				\
									\
      _message_ << (_text_)						\
                << " (" << __FILE__ << ":" << __LINE__ << " # "		\
                << __FUNCTION__ << ")";					\
									\
      elle::core::String	_string_(_message_.str());		\
									\
      elle::misc::report.Notify(_type_, _string_);			\
    } while (false)

///
/// this macro-function is used for successfully returning from
/// a normal method or function.
///
#define leave()								\
  return (elle::misc::StatusOk)

///
/// this macro-function just returns StatusTrue.
///
#define true()								\
  return (elle::misc::StatusTrue)

///
/// this macro-function just returns StatusFalse.
///
#define false()								\
  return (elle::misc::StatusFalse)

///
/// this macro-function reports a warning.
///
#define warn(_text_)							\
  report(elle::misc::Report::TypeWarning, _text_)

///
/// this macro-function indicates that an error occured
/// and returns StatusError.
///
#define escape(_text_)							\
  do									\
    {									\
      report(elle::misc::Report::TypeError, _text_);			\
									\
      return (elle::misc::StatusError);					\
    } while (false)

///
/// this macro-function indicates that an error occured
/// and return StatusFalse.
///
#define flee(_text_)							\
  do									\
    {									\
      report(elle::misc::Report::TypeError, _text_);			\
									\
      return (elle::misc::StatusFalse);					\
    } while (false)							\

///
/// this macro-function reports an error and returns.
///
/// note that the return object is specifed, hence this function
/// perfectly fits when an error occurs in operators etc.
///
#define yield(_text_, _return_)						\
  do									\
    {									\
      report(elle::misc::Report::TypeError, _text_);			\
									\
      return (_return_);						\
    } while (false)

///
/// this macro-function reports an error and returns.
///
/// note that the return object is specifed, hence this function
/// perfectly fits when an error occurs in constructors etc.
///
#define notify(_text_)							\
  do									\
    {									\
      report(elle::misc::Report::TypeError, _text_);			\
									\
      return;								\
    } while (false)

///
/// this macro-function adds an failure, displays the stack and
/// stops the program.
///
#define fail(_text_)							\
  do									\
    {									\
      report(elle::misc::Report::TypeFailure, _text_);			\
									\
      show();								\
									\
      ::exit(EXIT_FAILURE);						\
    } while (false)

///
/// this macro-function displays the error stack on the error output.
///
#define show()								\
  std::cerr << elle::misc::report

///
/// this macro-function, in the case of reported errors, displays them
/// and exits. otherwise, the function does not do anything.
///
#define expose()							\
  do									\
    {									\
      if (elle::misc::report.store.empty() == false)			\
        {								\
          show();							\
									\
	  ::exit(EXIT_FAILURE);						\
        }								\
    } while (false)

///
/// this macro-function flushes all the recorded messages.
///
#define purge()								\
  do									\
    {									\
      elle::misc::report.Flush();					\
    } while (false)

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
      void		Flush();
      void		Notify(Type,
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
