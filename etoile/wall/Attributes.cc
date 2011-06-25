//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Attributes.cc
//
// created       julien quintard   [wed mar 31 19:26:06 2010]
// updated       julien quintard   [sat jun 25 16:23:53 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Attributes.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Gear.hh>

#include <etoile/automaton/Attributes.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method sets an attribute for the given object.
    ///
    elle::Status	Attributes::Set(
			  const gear::Identifier&		identifier,
			  const elle::String&			name,
			  const elle::String&			value)
    {
      gear::Scope*	scope;
      gear::Object*	context;

      enter();

      printf("[XXX] Attributes::Set()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the set automaton on the context.
      if (automaton::Attributes::Set(*context,
				     name,
				     value) == elle::StatusError)
	escape("unable to set the attribute");

      leave();
    }

    ///
    /// this method returns the caller the trait associated with
    /// the given name.
    ///
    elle::Status	Attributes::Get(
			  const gear::Identifier&		identifier,
			  const elle::String&			name,
			  nucleus::Trait&			trait)
    {
      gear::Scope*	scope;
      gear::Object*	context;

      enter();

      printf("[XXX] Attributes::Get()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the get automaton on the context.
      if (automaton::Attributes::Get(*context,
				     name,
				     trait) == elle::StatusError)
	escape("unable to get the attribute");

      leave();
    }

    ///
    /// this method returns all the attributes.
    ///
    elle::Status	Attributes::Fetch(
			  const gear::Identifier&		identifier,
			  nucleus::Range<nucleus::Trait>&	range)
    {
      gear::Scope*	scope;
      gear::Object*	context;

      enter();

      printf("[XXX] Attributes::Fetch()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the fetch automaton on the context.
      if (automaton::Attributes::Fetch(*context,
				       range) == elle::StatusError)
	escape("unable to fetch the attribute");

      leave();
    }

    ///
    /// this method removes the given attribute from the list.
    ///
    elle::Status	Attributes::Omit(
			  const gear::Identifier&		identifier,
			  const elle::String&			name)
    {
      gear::Scope*	scope;
      gear::Object*	context;

      enter();

      printf("[XXX] Attributes::Omit()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the omit automaton on the context.
      if (automaton::Attributes::Omit(*context,
				      name) == elle::StatusError)
	escape("unable to omit the attribute");

      leave();
    }

  }
}
