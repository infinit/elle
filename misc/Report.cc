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
// updated       julien quintard   [sat mar 20 04:47:47 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Report.hh>

///
/// these includes are placed here in order to prevent pre-processing
/// conflicts.
///
#include <elle/archive/Archive.hh>

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

	  entry = this->store.front();

	  delete entry;

	  this->store.pop_front();
	}
    }

    ///
    /// this method adds a message to the report.
    ///
    Void		Report::Record(Report::Type		type,
				       const String&		text)
    {
      Report::Entry*	entry;

      // allocate the entry.
      entry = new Report::Entry;

      // set the fields.
      entry->type = type;
      entry->message = text;

      this->store.push_front(entry);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the report.
    ///
    Status		Report::Dump(const Natural32		margin) const
    {
      Report::Scoutor	scoutor;
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Report]" << std::endl;

      // go through the container.
      for (scoutor = this->store.begin();
	   scoutor != this->store.end();
	   scoutor++)
	{
	  Report::Entry*	entry = *scoutor;

	  // display the entry.
	  std::cout << alignment << shift << "[Entry] ("
		    << entry->type << ") " << entry->message << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a report.
    ///
    Status		Report::Serialize(Archive&		archive) const
    {
      Report::Scoutor	scoutor;

      enter();

      // serialize the number of messages.
      if (archive.Serialize((Natural32)this->store.size()) == StatusError)
	escape("unable to serialize the number of messages");

      // go through the container.
      for (scoutor = this->store.begin();
	   scoutor != this->store.end();
	   scoutor++)
	{
	  // serialize the entry.
	  if (archive.Serialize((Byte&)(*scoutor)->type,
				(*scoutor)->message) == StatusError)
	    escape("unable to serialize the entry");
	}

      leave();
    }

    ///
    /// this method extracts a report.
    ///
    Status		Report::Extract(Archive&		archive)
    {
      Natural32		size;
      Natural32		i;

      enter();

      // extract the number of messages.
      if (archive.Extract((Natural32&)size) == StatusError)
	escape("unable to extract the number of messages");

      // iterate and recreate the messages.
      for (i = 0; i < size; i++)
	{
	  Report::Entry*	entry;

	  // allocate a new entry.
	  entry = new Report::Entry;

	  // extract the entry.
	  if (archive.Serialize((Byte&)entry->type,
				entry->message) == StatusError)
	    escape("unable to serialize the entry");
	}

      leave();
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
	elle::misc::Report::Entry*	entry = report.store.front();
	elle::core::String		alignment(i, ' ');

	stream << alignment;

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

	stream << entry->message << std::endl;

	delete entry;

	report.store.pop_front();
      }

    return (stream);
  }

}
