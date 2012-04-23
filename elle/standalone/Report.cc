#include <elle/standalone/Report.hh>
#include <elle/standalone/Region.hh>
#include <elle/standalone/Maid.hh>

#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Program.hh>

namespace elle
{

  using namespace concurrency;

  namespace standalone
  {

#include <elle/idiom/Close.hh>
    reactor::LocalStorage<Report> Report::report(concurrency::scheduler());
#include <elle/idiom/Open.hh>

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the report by allocating a default
    /// report object.
    ///
    Status              Report::Initialize()
    {
      return Status::Ok;
    }

    ///
    /// this method cleans the report system.
    ///
    Status              Report::Clean()
    {
      return Status::Ok;
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
    Report::Report(const Report&                                report)
    {
      Report::Scoutor   scoutor;

      // go through the container.
      for (scoutor = report.container.begin();
           scoutor != report.container.end();
           scoutor++)
        {
          Report::Entry*        entry = *scoutor;

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
    Void                Report::Flush()
    {
      // go through all the remaining items.
      while (this->container.empty() == false)
        {
          Report::Entry*        entry;

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
    Void                Report::Record(const String&            location,
                                       const String&            time,
                                       const String&            message)
    {
      Report::Entry*    entry;

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
    Void                Report::Record(const Report&            report)
    {
      Report::Scoutor   scoutor;

      // go through the record and record every message.
      for (scoutor = report.container.begin();
           scoutor != report.container.end();
           scoutor++)
        {
          Report::Entry*        entry = *scoutor;

          // create the message by prepending a two-character margin.
          entry->message = entry->message;

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
    Status              Report::Dump(const Natural32            margin) const
    {
      Report::Scoutor   scoutor;
      String            alignment(margin, ' ');

      std::cout << alignment << "[Report]" << std::endl;

      // go through the container.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          Report::Entry*        entry = *scoutor;

          // display the entry.
          std::cout << "  "
                    << entry->message
                    << " ("
                    << entry->location
                    << ") @ "
                    << entry->time
                    << std::endl;
        }

      return Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a report.
    ///
    //Status              Report::Serialize(Archive&              archive) const
    //{
    //  Report::Scoutor   scoutor;

    //  // serialize the number of messages.
    //  if (archive.Serialize(
    //        static_cast<Natural32>(this->container.size())) == Status::Error)
    //    escape("unable to serialize the number of messages");

    //  // go through the container.
    //  for (scoutor = this->container.begin();
    //       scoutor != this->container.end();
    //       scoutor++)
    //    {
    //      Report::Entry*        entry = *scoutor;

    //      // serialize the entry.
    //      if (archive.Serialize(entry->location,
    //                            entry->time,
    //                            entry->message) == Status::Error)
    //        escape("unable to serialize the entry");
    //    }

    //  return Status::Ok;
    //}

    /////
    ///// this method extracts a report.
    /////
    //Status              Report::Extract(Archive&                archive)
    //{
    //  Natural32         size;
    //  Natural32         i;

    //  // extract the number of messages.
    //  if (archive.Extract(size) == Status::Error)
    //    escape("unable to extract the number of messages");

    //  // iterate and recreate the messages.
    //  for (i = 0; i < size; i++)
    //    {
    //      Report::Entry*        entry;

    //      // allocate a new entry.
    //      entry = new Report::Entry;

    //      // extract the entry.
    //      if (archive.Extract(entry->location,
    //                          entry->time,
    //                          entry->message) == Status::Error)
    //        escape("unable to serialize the entry");

    //      // push the extract entry in the container.
    //      this->container.push_front(entry);
    //    }

    //  return Status::Ok;
    //}

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method returns the size of the report object.
    ///
    Status              Report::Imprint(Natural32&              size) const
    {
      // return the size.
      size = sizeof (Report);

      return Status::Ok;
    }

    ///
    /// this method clones the current report.
    ///
    Status              Report::Clone(Report*&                  object) const
    {
      // allocate the object.
      object = new Report(*this);

      return Status::Ok;
    }

    ///
    /// this method check if two reports match.
    ///
    Boolean             Report::operator==(const Report&) const
    {
      //XXX
      throw("this method should never have been called");
    }

    ///
    /// this operator compares two reports.
    ///
    Boolean             Report::operator<(const Report&) const
    {
      //XXX
      throw("this method should never have been called");
    }

    ///
    /// this operator compares two reports.
    ///
    Boolean             Report::operator>(const Report&) const
    {
      //XXX
      throw("this method should never have been called");
    }

    ///
    /// this method copies a report.
    ///
    Report&             Report::operator=(const Report&         element)
    {
      // test if the reports are identical.
      if (this == &element)
        return (*this);

      // recycle the report.
      if (this->Recycle(&element) == Status::Error)
        yield(*this, "unable to recycle the report");

      return (*this);
    }

    ///
    /// this operator compares two reports. the source code of this
    /// function can be automatically generated through the Embed(Report, T)
    /// macro function.
    ///
    Boolean             Report::operator!=(const Report&        element) const
    {
      return (!(*this == element));
    }

    ///
    /// this operator compares two reports.
    ///
    Boolean             Report::operator<=(const Report&) const
    {
      //XXX
      throw("this method should never have been called");
    }

    ///
    /// this operator compares two reports.
    ///
    Boolean             Report::operator>=(const Report&) const
    {
      //XXX
      throw("this method should never have been called");
    }

  }
}
