//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Gear.cc
//
// created       julien quintard   [tue jun 14 16:41:06 2011]
// updated       julien quintard   [tue jun 14 20:53:04 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Gear.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the set of exported scopes.
    ///
    Gear::Container			Gear::Scopes;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method adds the given scope to the container in order to
    /// be retrieved at a later time.
    ///
    elle::Status	Gear::Add(Identifier&		identifier,
				  Scope*		scope)
    {
      std::pair<Gear::Iterator, elle::Boolean>	result;

      enter();

      // check if this identifier has already been recorded.
      if (Gear::Scopes.find(identifier) != Gear::Scopes.end())
	escape("this identifier seems to have already been recorded");

      // insert the scope in the container.
      result = Gear::Scopes.insert(
	         std::pair<const Identifier, Scope*>(identifier, scope));

      // check the result.
      if (result.second == false)
	escape("unable to insert the context in the container");

      leave();
    }

    ///
    /// this method returns the scope associated with the given identifier.
    ///
    elle::Status	Gear::Select(Identifier&	identifier,
				     Scope*&		scope)
    {
      Gear::Scoutor	scoutor;

      enter();

      // find the entry.
      if ((scoutor = Gear::Scopes.find(identifier)) == Gear::Scopes.end())
	escape("unable to locate the scope associated with the identifier");

      // return the scope.
      scope = scoutor->second;

      leave();
    }

    ///
    /// this method removes a scope from the container, making it unusable
    /// at a later time.
    ///
    elle::Status	Gear::Remove(Identifier&	identifier)
    {
      Gear::Iterator	iterator;

      enter();

      // find the entry.
      if ((iterator = Gear::Scopes.find(identifier)) == Gear::Scopes.end())
	escape("unable to locate the scope associated with the identifier");

      // erase the entry.
      Gear::Scopes.erase(iterator);

      leave();
    }

  }
}
