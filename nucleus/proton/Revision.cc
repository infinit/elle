#include <nucleus/proton/Revision.hh>

#include <elle/standalone/Log.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <limits>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant represents the first revision.
    ///
    const Revision               Revision::First;

    ///
    /// this constant represents the latest revision.
    ///
    const Revision Revision::Last = std::numeric_limits<Revision::Type>::max();

    ///
    /// this constant represents any revision and is useful whenever
    /// dealing with immutable blocks for which revision do not make any
    /// sense.
    ///
    const Revision               Revision::Any(Revision::Last);

    ///
    /// this constant is an alias of Any.
    ///
    const Revision&              Revision::Some = Revision::Any;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Revision::Revision():
      number(0)
    {
    }

    ///
    /// specific constructor.
    ///
    Revision::Revision(const Type                                 number):
      number(number)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a revision.
    ///
    elle::Status        Revision::Create(const Type              number)
    {
      // assign the number.
      this->number = number;

      return elle::Status::Ok;
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Revision::operator ==(Revision const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      return (this->number == other.number);
    }

    elle::Boolean
    Revision::operator <(Revision const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      // compare the numbers.
      if (this->number >= other.number)
        return false;

      return true;
    }

    elle::Boolean
    Revision::operator >(Revision const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      // compare the numbers.
      if (this->number <= other.number)
        return false;

      return true;
    }

    Revision&
    Revision::operator +=(elle::Natural32 const increment)
    {
      // increment the number.
      this->number += increment;

      return (*this);
    }

    Revision
    Revision::operator +(Revision const& other) const
    {
      return (Revision(this->number + other.number));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the revision's internals.
    ///
    elle::Status        Revision::Dump(const elle::Natural32     margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Revision] " << this->number << std::endl;

      return elle::Status::Ok;
    }


    /*----------.
    | Printable |
    `----------*/

    void
    Revision::print(std::ostream& s) const
    {
      if (*this == First)
        s << "first";
      else if (*this == Last)
        s << "last";
      else if (*this == Any)
        s << "any";
      else
        s << this->number;
    }
  }
}
