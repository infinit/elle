//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/standalone/Report.cc
//
// created       julien quintard   [sun oct 28 19:11:07 2007]
// updated       julien quintard   [sat jul 30 22:08:44 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Report.hh>
#include <elle/standalone/Region.hh>
#include <elle/standalone/Maid.hh>

#include <elle/package/Archive.hh>

#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Fiber.hh>

namespace elle
{
  using namespace core;
  using namespace package;
  using namespace concurrency;

  namespace standalone
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
      Callback<
	Status,
	Parameters<const Phase, Fiber*> >	govern(&Report::Govern);

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

      // reset the pointer.
      Report::Current = NULL;

      leave();
    }

    ///
    /// this method returns the current instance of the report.
    ///
    /// note that this method returns true or false.
    ///
    Status		Report::Instance(Report*&		report)
    {
      enter();

      // verify the report's presence.
      if (Report::Current == NULL)
	false();

      report = Report::Current;

      true();
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
    /// default constructor.
    ///
    Report::Report()
    {
    }

    ///
    /// copy constructor.
    ///
    Report::Report(const Report&				report)
    {
      Report::Scoutor	scoutor;

      // go through the container.
      for (scoutor = report.container.begin();
	   scoutor != report.container.end();
	   scoutor++)
	{
	  Report::Entry*	entry = *scoutor;

	  // record the entry.
	  this->Record(entry->location,
		       entry->time,
		       entry->message);
	}
    }

    ///
    /// this destructor deletes the stack.
    ///
    Report::~Report()
    {
      // flush the report.
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
      while (this->container.empty() == false)
	{
	  Report::Entry*	entry;

	  // retrieve the front item.
	  entry = this->container.front();

	  // delete the item.
	  delete entry;

	  // remove it.
	  this->container.pop_front();
	}
    }

    ///
    /// this method adds a message to the report.
    ///
    Void		Report::Record(const String&		location,
				       const String&		time,
				       const String&		message)
    {
      Report::Entry*	entry;

      // allocate the entry.
      entry = new Report::Entry;

      // set the fields.
      entry->location = location;
      entry->time = time;
      entry->message = message;

      // store the entry in the container.
      this->container.push_front(entry);
    }

    ///
    /// this method adds a set of messages to the report.
    ///
    Void		Report::Record(const Report&		report)
    {
      Report::Scoutor	scoutor;

      // go through the record and record every message.
      for (scoutor = report.container.begin();
	   scoutor != report.container.end();
	   scoutor++)
	{
	  Report::Entry*	entry = *scoutor;

	  // create the message by prepending a two-character margin.
	  entry->message = String("  ") + entry->message;

	  // store the entry.
	  this->Record(entry->location,
		       entry->time,
		       entry->message);
	}
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
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Report::Entry*	entry = *scoutor;

	  // display the entry.
	  std::cout << "  "
		    << entry->message
		    << " ("
		    << entry->location
		    << ") @ "
		    << entry->time
		    << std::endl;
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
      if (archive.Serialize((Natural32)this->container.size()) == StatusError)
	escape("unable to serialize the number of messages");

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Report::Entry*	entry = *scoutor;

	  // serialize the entry.
	  if (archive.Serialize(entry->location,
				entry->time,
				entry->message) == StatusError)
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

	  // allocate a new entry.
	  entry = new Report::Entry;

	  // extract the entry.
	  if (archive.Extract(entry->location,
			      entry->time,
			      entry->message) == StatusError)
	    escape("unable to serialize the entry");

	  // push the extract entry in the container.
	  this->container.push_front(entry);
	}

      leave();
    }

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method returns the size of the report object.
    ///
    Status		Report::Imprint(Natural32&		size) const
    {
      enter();

      // return the size.
      size = sizeof (Report);

      leave();
    }

    ///
    /// this method clones the current report.
    ///
    Status		Report::Clone(Report*&			object) const
    {
      enter();

      // allocate the object.
      object = new Report(*this);

      leave();
    }

    ///
    /// this method check if two reports match.
    ///
    Boolean		Report::operator==(const Report&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // XXX

      true();
    }

    ///
    /// this operator compares two reports.
    ///
    Boolean		Report::operator<(const Report&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this operator compares two reports.
    ///
    Boolean		Report::operator>(const Report&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this method copies a report.
    ///
    Report&		Report::operator=(const Report&		element)
    {
      enter();

      // test if the reports are identical.
      if (this == &element)
	return (*this);

      // recycle the report.
      if (this->Recycle(&element) == StatusError)
	yield(*this, "unable to recycle the report");

      return (*this);
    }

    ///
    /// this operator compares two reports. the source code of this
    /// function can be automatically generated through the Embed(Report, T)
    /// macro function.
    ///
    Boolean		Report::operator!=(const Report&	element) const
    {
      return (!(*this == element));
    }

    ///
    /// this operator compares two reports.
    ///
    Boolean		Report::operator<=(const Report&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this operator compares two reports.
    ///
    Boolean		Report::operator>=(const Report&) const
    {
      enter();

      flee("this method should never have been called");
    }

  }
}
