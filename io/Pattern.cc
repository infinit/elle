//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       elle
//
// author        julien quintard   [mon apr 25 11:37:13 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/io/Pattern.hh>

namespace elle
{
  using namespace standalone;

  namespace io
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a pattern.
    ///
    Status		Pattern::Create(const String&		string)
    {
      enter();

      // assign the string.
      this->string = string;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two objects match.
    ///
    Boolean		Pattern::operator==(const Pattern&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the internal.
      if (this->string != element.string)
	false();

      true();
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Pattern, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the object's internal.
    ///
    Status		Pattern::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Pattern] "
		<< this->string << std::endl;

      leave();
    }

  }
}
