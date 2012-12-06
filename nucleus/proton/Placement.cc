#include <nucleus/proton/Placement.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable holds the current placement counter.
    ///
    const elle::Natural64               Placement::Zero = 0;

    ///
    /// this variable defines an unused hence null Placement.
    ///
    const Placement                     Placement::Null;

    ///
    /// this variable holds the current placement counter.
    ///
    elle::Natural64                     Placement::counter =
      Placement::Zero + 1;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method generates a new unique placement.
    ///
    Placement
    Placement::generate()
    {
      Placement placement(Placement::counter++);

      return (placement);
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Placement::Placement():
      _number(Placement::Zero)
    {
    }

    ///
    /// specific constructor.
    ///
    Placement::Placement(const elle::Natural64 number):
      _number(number)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    elle::Natural64
    Placement::number() const
    {
      return (this->_number);
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Placement::operator ==(Placement const& other) const
    {
      if (this == &other)
        return true;

      return (this->_number == other.number());
    }

    elle::Boolean
    Placement::operator <(Placement const& other) const
    {
      return (this->_number < other.number());
    }

    elle::Boolean
    Placement::operator <=(Placement const& other) const
    {
      return (this->_number <= other.number());
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an placement.
    ///
    elle::Status        Placement::Dump(const elle::Natural32   margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Placement] "
                << std::dec << this->_number << std::endl;

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Placement::print(std::ostream& stream) const
    {
      stream << this->_number;
    }
  }
}
