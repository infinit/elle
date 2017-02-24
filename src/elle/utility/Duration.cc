#include <elle/utility/Duration.hh>
#include <elle/io/Dumpable.hh>

#include <iostream>

namespace elle
{
  namespace utility
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Duration::Duration():
      unit(Duration::UnitUnknown),
      value(0)
    {
    }

    ///
    /// common constructor.
    ///
    Duration::Duration(const Unit                               unit,
                       const Natural64                          value):
      unit(unit),
      value(value)
    {
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    bool Duration::operator==(const Duration&    element) const
    {
      // compare the attributes.
      if ((this->unit != element.unit) ||
          (this->value != element.value))
        return false;

      return true;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an duration object.
    ///
    Status              Duration::Dump(Natural32                margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Duration]" << std::endl;

      std::cout << alignment << io::Dumpable::Shift << "[Unit] "
                << std::nouppercase << std::dec
                << this->unit << std::endl;
      std::cout << alignment << io::Dumpable::Shift << "[Value] "
                << std::nouppercase << std::dec
                << this->value << std::endl;

      return Status::Ok;
    }

  }
}
