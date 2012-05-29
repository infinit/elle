#include <iostream>
#include <sstream>

# include <elle/standalone/Log.hh>
# include <elle/standalone/Report.hh>

#include <etoile/gear/Identifier.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable holds the current identifier counter.
    ///
    const elle::Natural64               Identifier::Zero = 0;

    ///
    /// this variable defines an unused hence null Identifier.
    ///
    const Identifier                    Identifier::Null;

    ///
    /// this variable holds the current identifier counter.
    ///
    elle::Natural64                     Identifier::Counter =
      Identifier::Zero + 1;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Identifier::Identifier():
      value(Identifier::Zero)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates a new unique identifier.
    ///
    elle::Status        Identifier::Generate()
    {
      // increments the counter.
      this->value = Identifier::Counter++;

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two identifiers match.
    ///
    elle::Boolean       Identifier::operator==(const Identifier& element)
      const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the identifier.
      if (this->value != element.value)
        return false;

      return true;
    }

    ///
    /// this operator compares two identifiers.
    ///
    elle::Boolean       Identifier::operator<(const Identifier& element) const
    {
      return (this->value < element.value);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Identifier, _());

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the identifier.
    ///
    //elle::Status        Identifier::Serialize(elle::Archive&    archive) const
    //{
    //  // serialize the attributes.
    //  if (archive.Serialize(this->value) == elle::Status::Error)
    //    escape("unable to serialize the identifier attributes");

    //  return elle::Status::Ok;
    //};

    /////
    ///// this method extracts the identifier.
    /////
    //elle::Status        Identifier::Extract(elle::Archive&      archive)
    //{
    //  // extract the attributes.
    //  if (archive.Extract(this->value) == elle::Status::Error)
    //    escape("unable to extract the identifier attributes");

    //  return elle::Status::Ok;
    //};

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an identifier.
    ///
    elle::Status        Identifier::Dump(const elle::Natural32  margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Identifier] "
                << std::dec << this->value << std::endl;

      return elle::Status::Ok;
    }

  }
}
