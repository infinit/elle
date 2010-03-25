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
// updated       julien quintard   [thu mar 25 17:39:37 2010]
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
    /// this method adds a text message to the report.
    ///
    Void		Report::Record(Report::Type		type,
				       const String&		meta,
				       const char*		text)
    {
      String		message(text);

      // record the message.
      Report::Record(type, meta, message);
    }

    ///
    /// this method adds a message to the report.
    ///
    Void		Report::Record(Report::Type		type,
				       const String&		meta,
				       const String&		text)
    {
      Report::Entry*	entry;

      // allocate the entry.
      entry = new Report::Entry;

      // set the fields.
      entry->type = type;
      entry->meta = meta;
      entry->message = text;

      this->store.push_front(entry);
    }

    ///
    /// this method adds a set of messages to the report.
    ///
    Void		Report::Record(Report::Type		type,
				       const String&		meta,
				       const Report&		report)
    {
      Report::ReverseScoutor	scoutor;

      // go through the record and record every message.
      for (scoutor = report.store.rbegin();
	   scoutor != report.store.rend();
	   scoutor++)
	{
	  Report::Entry*	entry = *scoutor;
	  String		message;

	  // create the message by prepending a margin.
	  message.append("  ");
	  message.append(entry->message);

	  // record the message.
	  Report::Record(entry->type, entry->meta, message);
	}

      // create a headline message of the given type.
      Report::Record(type, meta, "Report");
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

      enter();

      std::cout << alignment << "[Report]" << std::endl;

      // go through the container.
      for (scoutor = this->store.begin();
	   scoutor != this->store.end();
	   scoutor++)
	{
	  Report::Entry*	entry = *scoutor;

	  // display the error type.
	  switch (entry->type)
	    {
	    case elle::misc::Report::TypeWarning:
	      {
		std::cout << alignment << Dumpable::Shift << "[Warning] ";
		break;
	      }
	    case elle::misc::Report::TypeError:
	      {
		std::cout << alignment << Dumpable::Shift << "[Error] ";
		break;
	      }
	    case elle::misc::Report::TypeFailure:
	      {
		std::cout << alignment << Dumpable::Shift << "[Failure] ";
		break;
	      }
	    default:
	      {
		std::cout << alignment << Dumpable::Shift << "[Unknown] ";
		break;
	      }
	    }

	  // display the entry.
	  std::cout << entry->message << " ("
		    << entry->meta << ")" << std::endl;
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
				(*scoutor)->meta,
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
      if (archive.Extract(size) == StatusError)
	escape("unable to extract the number of messages");

      // iterate and recreate the messages.
      for (i = 0; i < size; i++)
	{
	  Report::Entry*	entry;
	  Byte			type;

	  // allocate a new entry.
	  entry = new Report::Entry;

	  // extract the entry.
	  if (archive.Extract(type,
			      entry->meta,
			      entry->message) == StatusError)
	    escape("unable to serialize the entry");

	  entry->type = (Report::Type)type;

	  // push the extract entry in the container.
	  this->store.push_front(entry);
	}

      leave();
    }

  }
}
