//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       elle
//
// file          /home/mycure/infinit/elle/io/Piece.cc
//
// created       julien quintard   [mon apr 25 11:56:16 2011]
// updated       julien quintard   [tue apr 26 12:19:19 2011]
//

//
// ---------- include ---------------------------------------------------------
//

#include <elle/io/Piece.hh>

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
    Piece::Piece(const String&					name,
		 const String&					value):
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
    Boolean		Piece::operator==(const Piece&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the internal.
      if ((this->name != element.name) || (this->value != element.value))
	false();

      true();
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
    Status		Piece::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Piece] "
		<< this->name
		<< ": "
		<< this->value
		<< std::endl;

      leave();
    }

  }
}
