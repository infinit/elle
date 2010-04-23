//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/miscellaneous/Report.cc
//
// created       julien quintard   [sun oct 28 19:11:07 2007]
// updated       julien quintard   [wed apr 14 11:50:40 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Report.hh>

///
/// these includes are placed here in order to prevent pre-processing
/// conflicts.
///
#include <elle/archive/Archive.hh>
#include <elle/concurrency/Fiber.hh>

namespace elle
{
  using namespace core;

  namespace miscellaneous
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this global variable represent the report of the current thread/fiber.
    ///
    Report*		Report::Current = NULL;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the report by allocating a default
    /// report object.
    ///
    Status		Report::Initialize()
    {
      Callback<const Phase, Fiber*>	govern(&Report::Govern);

      enter();

      // allocate the report for the initial thread/fiber.
      Report::Current = new Report;

      // register the govern callback to the fiber system.
      if (Fiber::Register(govern) == StatusError)
	escape("unable to register the govern callback");

      leave();
    }

    ///
    /// this method cleans the report system.
    ///
    Status		Report::Clean()
    {
      enter();

      // delete the report.
      delete Report::Current;

      leave();
    }

    ///
    /// this method returns the current instance of the report.
    ///
    Status		Report::Instance(Report*&		report)
    {
      enter();

      // return the report.
      report = Report::Current;

      leave();
    }

    ///
    /// this method initializes, saves, restores and cleans the report for
    /// the given fiber, in other words govern the fiber's environment.
    ///
    Status		Report::Govern(const Phase&		phase,
				       Fiber*&			fiber)
    {
      enter();

      // perform an operation depending on the phase.
      switch (phase)
	{
	case PhaseInitialize:
	  {
	    // allocate a new report and install it.
	    Report::Current = new Report;

	    break;
	  }
	case PhaseSave:
	  {
	    // save the report in the fiber's environment.
	    if (fiber->environment->Store("report",
					  Report::Current) == StatusError)
	      escape("unable to store the report in the environment");

	    // set the pointer to NULL, for safety purposes.
	    Report::Current = NULL;

	    break;
	  }
	case PhaseRestore:
	  {
	    // load the report from the environment.
	    if (fiber->environment->Load("report",
					 Report::Current) == StatusError)
	      escape("unable to load the report from the environment");

	    break;
	  }
	case PhaseClean:
	  {
	    // delete the report.
	    delete Report::Current;

	    // reinitialize the report to NULL, for safety purposes.
	    Report::Current = NULL;

	    break;
	  }
	}

      leave();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this destructor deletes the stack.
    ///
    Report::~Report()
    {
      this->Flush();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method flushes the report.
    ///
    Void		Report::Flush()
    {
      // go through all the remaining items.
      while (this->store.empty() == false)
	{
	  Report::Entry*	entry;

	  // retrieve the front item.
	  entry = this->store.front();

	  // delete the item.
	  delete entry;

	  // remove it.
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
      this->Record(type, meta, message);
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
      Report::Scoutor	scoutor;

      // go through the record and record every message.
      for (scoutor = report.store.begin();
	   scoutor != report.store.end();
	   scoutor++)
	{
	  Report::Entry*	entry = *scoutor;
	  String		message;

	  // create the message by prepending a margin.
	  message.append("  ");
	  message.append(entry->message);

	  // record the message.
	  this->Record(entry->type, entry->meta, message);
	}

      // create a headline message of the given type.
      this->Record(type, meta, "Report");
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
	    case Report::TypeWarning:
	      {
		std::cout << alignment << Dumpable::Shift << "[Warning] ";
		break;
	      }
	    case Report::TypeError:
	      {
		std::cout << alignment << Dumpable::Shift << "[Error] ";
		break;
	      }
	    case Report::TypeFailure:
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

	  if (entry->meta.length() != 0)
	    {
	      // display the entry.
	      std::cout << entry->message << " ("
			<< entry->meta << ")" << std::endl;
	    }
	  else
	    {
	      // display the entry without the meta.
	      std::cout << entry->message << std::endl;
	    }
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
	  if (archive.Serialize((Natural8&)(*scoutor)->type,
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
	  Natural8		type;

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
