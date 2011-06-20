//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Scope.hxx
//
// created       julien quintard   [wed jun 15 13:09:29 2011]
// updated       julien quintard   [fri jun 17 16:52:50 2011]
//

#ifndef ETOILE_GEAR_SCOPE_HXX
#define ETOILE_GEAR_SCOPE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Gear.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename T>
    Scope<T>::Scope():
      chronicle(NULL)
    {
    }

    ///
    /// destructor.
    ///
    template <typename T>
    Scope<T>::~Scope()
    {
      // delete the chronicle.
      if (this->chronicle != NULL)
	delete this->chronicle;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method exports the scope so that Etoile can retrieve the scope
    /// given its identifier.
    ///
    template <typename T>
    elle::Status	Scope<T>::Export()
    {
      enter();

      // check if this scope already has an identifier.
      if (this->identifier != Identifier::Null)
	escape("this scope seems to have been exported already");

      // generate an identifier.
      if (this->identifier.Generate() == elle::StatusError)
	escape("unable to generate the identifier");

      // add the scope to the gear container.
      if (Gear::Add(this->identifier, this) == elle::StatusError)
	escape("unable to add the scope to the gear container");

      leave();
    }

    ///
    /// XXX
    ///
    template <typename T>
    elle::Status	Scope<T>::Import()
    {
      enter();

      // check if this scope has an identifier.
      if (this->identifier == Identifier::Null)
	escape("this scope does not seem to have an identifier");

      // remove the scope from the gear container.
      if (Gear::Remove(this->identifier) == elle::StatusError)
	escape("unable to remove the scope from the gear container");

      leave();
    }

  }
}

#endif
