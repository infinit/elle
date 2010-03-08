//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Macro.hh
//
// created       julien quintard   [mon mar  8 23:05:41 2010]
// updated       julien quintard   [mon mar  8 23:06:10 2010]
//

//
// ---------- macro-functions -------------------------------------------------
//

///
/// these macro functions are used to make using the Elle library
/// functionalities easier.
///

///
/// this macro-function register a report entry.
///
/// note that this macro function should never be called directly. instead
/// the macro functions below should be used: leave, escape, true, alert
/// etc.
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
  do									\
    {									\
      release();							\
									\
      return (elle::misc::StatusOk);					\
    } while (false)

///
/// this macro-function just returns StatusTrue.
///
#define true()								\
  do									\
    {									\
      release();							\
									\
      return (elle::misc::StatusTrue);					\
    } while (false)

///
/// this macro-function just returns StatusFalse.
///
#define false()								\
  do									\
    {									\
      release();							\
									\
      return (elle::misc::StatusFalse);					\
    } while (false)

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
      release();							\
									\
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
      release();							\
									\
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
      release();							\
      									\
      report(elle::misc::Report::TypeError, _text_);			\
									\
      return (_return_);						\
    } while (false)

///
/// this macro-function reports an error and returns.
///
/// XXX \todo this one should directly print on stderr or later
///     forward the message to a log application.
///
#define alert(_text_)							\
  do									\
    {									\
      release();							\
									\
      report(elle::misc::Report::TypeError, _text_);			\
									\
      show();								\
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
// ---------- macro functions -------------------------------------------------
//

///
/// this macro function allocates memory within the current scope through
/// the use of alloca() before creating a Garrison of Guards.
///
#define enter(_guards_...)						\
  Maid::Garrison* _maid_ =						\
    Maid::Install((Void*)alloca(sizeof(Maid::Garrison)), ##_guards_)

///
/// this macro wraps a local non-pointer variable by creating a pointer
/// of the same type. this pointer will then be used by the Maid though
/// the developer has to specifically inform the system regarding
/// when to start and stop tracking the variable through the track()
/// and untrack() macro-functions.
///
#define wrap(_name_)							\
  typeof(_name_)*	_ ## _name_ ## _

///
/// this macro function emulates a slab tracking but for local
/// non-pointed variables.
///
#define local(_name_, _function_)					\
  slab(_ ## _name_ ## _, _function_)

///
/// this macro-function starts the tracking by setting it to the
/// variable's address.
///
#define track(_name_)							\
  _ ## _name_ ## _ = &_name_

///
/// unlike the previous one, this macro-function sets the pointer
/// to NULL, hence stopping the tracking process.
///
#define untrack(_name_)							\
  _ ## _name_ ## _ = NULL

///
/// this macro-function is similar to the previous one but in the
/// general case of pointers.
///
#define waive(_pointer_)						\
  _pointer_ = NULL

///
/// this macro-function creates a guard for class instances which will
/// then be relased through the delete statement.
///
#define instance(_pointer_)						\
  Maid::Monitor((Void*)alloca(sizeof(Maid::Instance<Void*>)),		\
                _pointer_)

///
/// this macro-function creates a guard for slab i.e memory-based variables
/// allocated via specific functions such as malloc(), RSA_new() etc.
///
#define slab(_pointer_, _function_)					\
  Maid::Monitor((Void*)alloca(sizeof(Maid::Slab<Void*,			\
                                                Void (*)(Void*)>)),	\
                _pointer_, _function_)

///
/// this macro-function releases the guarded objects cf Maid.
///
#define release()							\
  do									\
    {									\
      if (_maid_ != NULL)						\
	{								\
	  delete _maid_;						\
									\
	  _maid_ = NULL;						\
	}								\
    } while (false)

///
/// this macro function enables the caller to embed source code
/// automatically for a specific type. this functionality is used
/// by the Entity class for instance.
///
#define embed(_identifier_, _type_)					\
  _identifier_ ## Embed(_type_)
