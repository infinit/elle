//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Attributes.cc
//
// created       julien quintard   [mon feb  1 19:24:19 2010]
// updated       julien quintard   [mon apr 19 19:39:19 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Attributes.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets a trait.
    ///
    Status		Attributes::Set(context::Object*	context,
					const String&		name,
					const String&		value)
    {
      user::User*	user;

      enter();

      // determine the rights over the object.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // verify that the user can modify the attributes.
      if (context->rights->role != kernel::RoleOwner)
	escape("the object's owner only can modify the attributes");

      // test if the attribute already exists.
      if (context->object->meta.attributes.Exist(name) == StatusTrue)
	{
	  // update the trait, properly i.e by calling the Update() method.
	  if (context->object->meta.attributes.Update(name,
						      value) == StatusError)
	    escape("unable to update the trait");
	}
      else
	{
	  kernel::Trait*	trait;

	  // otherwise, create a new track, hence track it.
	  enter(instance(trait));

	  // allocate a new trait.
	  trait = new kernel::Trait(name, value);

	  // add the trait to the attributes.
	  if (context->object->meta.attributes.Add(trait) == StatusError)
	    escape("unable to add the trait");

	  // waive.
	  waive(trait);

	  // release the resources.
	  release();
	}

      // update the object since the attributes have changed.
      if (context->object->Administrate(
            context->object->meta.attributes,
	    context->object->meta.access,
	    context->object->meta.owner.permissions,
	    context->object->meta.owner.token) == StatusError)
	escape("unable to update the object's meta section");

      leave();
    }

    ///
    /// this method looks for the given name in the attributes and
    /// return the associated trait.
    ///
    Status		Attributes::Get(context::Object*	context,
					const String&		name,
					kernel::Trait*&		trait)
    {
      enter();

      // lookup in the attributes object.
      if (context->object->meta.attributes.Lookup(name, trait) == StatusError)
	escape("unable to lookup in the attributes");

      leave();
    }

    ///
    /// this method returns the attributes.
    ///
    Status		Attributes::Fetch(context::Object*	context,
					  kernel::Range<kernel::Trait>& range)
    {
      enter();

      // consult the attributes.
      if (context->object->meta.attributes.Consult(kernel::IndexFirst,
						   kernel::SizeMaximum,
						   range) == StatusError)
	escape("unable to fetch the attributes");

      leave();
    }

    ///
    /// this method removes a trait from the attributes.
    ///
    Status		Attributes::Omit(context::Object*	context,
					 const String&		name)
    {
      user::User*	user;

      enter();

      // determine the rights over the object.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // verify that the user can modify the attributes.
      if (context->rights->role != kernel::RoleOwner)
	escape("the object's owner only can modify the attributes");

      // remove the trait associated with the given name.
      if (context->object->meta.attributes.Remove(name) == StatusTrue)
	escape("unable to remove the trait");

      // update the object since the attributes have changed.
      if (context->object->Administrate(
            context->object->meta.attributes,
	    context->object->meta.access,
	    context->object->meta.owner.permissions,
	    context->object->meta.owner.token) == StatusError)
	escape("unable to update the object's meta section");

      leave();
    }

  }
}
