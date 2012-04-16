
#include <elle/io/Piece.hh>

#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace io
  {

//
// ---------- functions -------------------------------------------------------
//

    ///
    /// default constructor.
    ///
    Piece::Piece()
    {
    }

    ///
    /// the constructor.
    ///
    Piece::Piece(const String&                                  name,
                 const String&                                  value):
      name(name),
      value(value)
    {
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two objects match.
    ///
    Boolean             Piece::operator==(const Piece&          element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the internal.
      if ((this->name != element.name) || (this->value != element.value))
        return false;

      return true;
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Piece, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the object's internal.
    ///
    Status              Piece::Dump(const Natural32             margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Piece] "
                << this->name
                << ": "
                << this->value
                << std::endl;

      return Status::Ok;
    }

  }
}
