//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/miscellaneous/Report.hh
//
// created       julien quintard   [sun oct 28 19:12:38 2007]
// updated       julien quintard   [thu apr  8 20:18:54 2010]
//

#ifndef ELLE_MISCELLANEOUS_REPORT_HH
#define ELLE_MISCELLANEOUS_REPORT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>
#include <elle/core/Void.hh>

#include <elle/archive/Archivable.hh>

#include <elle/io/Dumpable.hh>

#include <elle/concurrency/Phase.hh>

#include <elle/idiom/Close.hh>
# include <sstream>
# include <list>
# include <ostream>
# include <iostream>
# include <string.h>
# include <assert.h>
# include <errno.h>
# include <stdlib.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace archive
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Archive;
  }

  namespace concurrency
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Fiber;
  }

  using namespace core;
  using namespace archive;
  using namespace io;
  using namespace concurrency;

  namespace miscellaneous
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
    class Report:
      public Dumpable, public Archivable
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
	String		meta;
	String		message;
      };

      //
      // types
      //
      typedef std::list<Entry*>			Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Instance(Report*&);

      static Status	Govern(const Phase&,
			       Fiber*&);

      //
      // static attributes
      //
      static Report*	Current;

      //
      // constructors & destructors
      //
      ~Report();

      //
      // methods
      //
      Void		Flush();
      Void		Record(Type,
			       const String&,
			       const char[]);
      Void		Record(Type,
			       const String&,
			       const String&);
      Void		Record(Type,
			       const String&,
			       const Report&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Container		store;
    };

  }
}

#endif
