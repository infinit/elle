//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed mar  3 13:55:58 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Event.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/cryptography/Random.hh>

namespace elle
{
  using namespace cryptography;

  namespace concurrency
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable defines an unused hence null Event.
    ///
    const Event                         Event::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Event::Event():
      identifier(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates a new unique event.
    ///
    Status              Event::Generate()
    {
      // try until the generated event is different from Null.
      do
        {
          // generate the identifier.
          if (Random::Generate(this->identifier) == Status::Error)
            escape("unable to generate the identifier");
        } while (*this == Event::Null);

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two events match.
    ///
    Boolean             Event::operator==(const Event&          element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return Status::True;

      // compare the identifier.
      if (this->identifier != element.identifier)
        return Status::False;

      return Status::True;
    }

    ///
    /// this operator compares two events.
    ///
    Boolean             Event::operator<(const Event&           element) const
    {
      return (this->identifier < element.identifier);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Event, _());

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the event.
    ///
    Status              Event::Serialize(Archive&               archive) const
    {
      // serialize the attributes.
      if (archive.Serialize(this->identifier) == Status::Error)
        escape("unable to serialize the event attributes");

      return Status::Ok;
    };

    ///
    /// this method extracts the event.
    ///
    Status              Event::Extract(Archive&                 archive)
    {
      // extract the attributes.
      if (archive.Extract(this->identifier) == Status::Error)
        escape("unable to extract the event attributes");

      return Status::Ok;
    };

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an event.
    ///
    Status              Event::Dump(const Natural32             margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Event] " << this->identifier << std::endl;

      return Status::Ok;
    }

  }
}
