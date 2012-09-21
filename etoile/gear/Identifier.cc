#include <etoile/gear/Identifier.hh>

#include <elle/standalone/Log.hh>
#include <elle/standalone/Report.hh>

#include <iostream>
#include <sstream>

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

      return (this->value == element.value);
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

    std::ostream&
    operator << (std::ostream& stream, Identifier const& id)
    {
      stream << id.value;
      return stream;
    }


    void
    Identifier::print(std::ostream& out) const
    {
      out << this->value;
    }

  }
}
