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
// updated       julien quintard   [thu apr 15 19:06:48 2010]
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
    /// this method adds a trait to the attributes.
    ///
    Status		Attributes::Add(context::Object*	context,
					const String&		name,
					const String&		value)
    {
      user::User*	user;
      kernel::Trait*	trait;

      enter(instance(trait));

      // determine the rights over the object.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // verify that the user can modify the attributes.
      if (context->rights->role != kernel::RoleOwner)
	escape("the object's owner only can modify the attributes");

      // look in the attributes object.
      if (context->object->meta.attributes.Exist(name) == StatusTrue)
	escape("this trait already exist in the attributes");

      // allocate a trait.
      trait = new kernel::Trait(name, value);

      // add the trait to the attributes.
      if (context->object->meta.attributes.Add(trait) == StatusError)
	escape("unable to add the trait");

      // waive.
      waive(trait);

      leave();
    }

    ///
    /// this method looks for the given name in the attributes and
    /// return the associated trait.
    ///
    Status		Attributes::Lookup(context::Object*	context,
					   const String&	name,
					   kernel::Trait*&	trait)
    {
      enter();

      // initialize the trait to NULL.
      trait = NULL;

      // check if the trait exists.
      if (context->object->meta.attributes.Exist(name) == StatusFalse)
	leave();

      // lookup in the attributes object.
      if (context->object->meta.attributes.Lookup(name, trait) == StatusError)
	escape("unable to lookup in the attributes");

      leave();
    }

    ///
    /// this method returns a subset---i.e a collection---of the attributes.
    ///
    Status		Attributes::Consult(context::Object*	context,
					    const kernel::Index& index,
					    const kernel::Size&	size,
					    kernel::Collection&	collection)
    {
      enter();

      // consult the attributes.
      if (context->object->meta.attributes.Consult(index,
						   size,
						   collection) == StatusError)
	escape("unable to consult the attributes");

      leave();
    }

    ///
    /// this method updates an existing trait with the given value.
    ///
    Status		Attributes::Update(context::Object*	context,
					   const String&	name,
					   const String&	value)
    {
      user::User*	user;
      kernel::Trait*	trait;

      enter();

      // determine the rights over the object.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // verify that the user can modify the attributes.
      if (context->rights->role != kernel::RoleOwner)
	escape("the object's owner only can modify the attributes");

      // retrieve the trait associated with the given name.
      if (context->object->meta.attributes.Lookup(name, trait) == StatusTrue)
	escape("unable to retrieve the attribute");

      // update the trait.
      trait->value = value;

      leave();
    }

    ///
    /// this method removes a trait from the attributes.
    ///
    Status		Attributes::Remove(context::Object*	context,
					   const String&	name)
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

      leave();
    }

  }
}
