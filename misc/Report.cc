//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/misc/Report.cc
//
// created       julien quintard   [sun oct 28 19:11:07 2007]
// updated       julien quintard   [fri mar  5 15:51:19 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Report.hh>

namespace elle
{
  using namespace core;

  namespace misc
  {

//
// ---------- globals ---------------------------------------------------------
//

    ///
    /// this global variable represent the report of the current thread.
    ///
    /// it can be dump through something like:
    ///
    ///   std::cerr << report << std::endl;
    ///
    /// the report is set as a global variable so that it is accessible from
    /// everywhere.
    ///
    /// \todo
    ///   in order to make the elle library reentrant, report variables would
    ///   need to be set as per-thread.
    ///
    Report		report;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this destructor deletes the stack.
    ///
    Report::~Report()
    {
      this->Flush();
    }

    ///
    /// this method flushes the report.
    ///
    Void		Report::Flush()
    {
      while (this->store.empty() == false)
	{
	  Report::Entry*	entry;

	  entry = this->store.top();

	  delete entry->message;
	  delete entry;

	  this->store.pop();
	}
    }

    ///
    /// this method adds a warning to the report.
    ///
    Void		Report::Notify(Report::Type		type,
				       const String&		text)
    {
      String*		message = new String(text);
      Entry*		entry = new Entry;

      entry->type = type;
      entry->message = message;

      this->store.push(entry);
    }

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this operator allows functions to easily dump the current report.
  ///
  /// this function flushes the report.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   elle::misc::Report&		report)
  {
    elle::core::Natural32	i;

    for (i = 0; report.store.empty() == false; i++)
      {
	elle::core::Natural32		j;
	elle::misc::Report::Entry*	entry = report.store.top();

	for (j = 0; j < i; j++)
	  stream << "  ";

	switch (entry->type)
	  {
	  case elle::misc::Report::TypeWarning:
	    {
	      stream << "[warning] ";
	      break;
	    }
	  case elle::misc::Report::TypeError:
	    {
	      stream << "[error] ";
	      break;
	    }
	  case elle::misc::Report::TypeFailure:
	    {
	      stream << "[failure] ";
	      break;
	    }
	  }

	stream << *entry->message << std::endl;

	delete entry->message;
	delete entry;

	report.store.pop();
      }

    return (stream);
  }

}
