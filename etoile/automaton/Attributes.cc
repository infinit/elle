#include <etoile/automaton/Attributes.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/gear/Object.hh>

#include <nucleus/neutron/Role.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Trait.hh>

#include <elle/idiom/Close.hh>
# include <limits>
#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets an attribute.
    ///
    elle::Status        Attributes::Set(
                          gear::Object&                         context,
                          const elle::String&                   name,
                          const elle::String&                   value)
    {

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // verify that the user can modify the attributes.
      if (context.rights.role != nucleus::neutron::RoleOwner)
        escape("the user does not seem to have the permission to update "
               "this object's attributes");

      // does the attribute already exist.
      if (context.object.meta.attributes.Exist(name) == elle::Status::True)
        {
          // update the trait, properly i.e by calling the Update() method.
          if (context.object.meta.attributes.Update(
                name,
                value) == elle::Status::Error)
            escape("unable to update the trait");
        }
      else
        {
          nucleus::neutron::Trait* trait;

          // allocate a new trait.
          trait = new nucleus::neutron::Trait(name, value);

          // add the trait to the attributes.
          if (context.object.meta.attributes.Add(trait) == elle::Status::Error)
            {
              delete trait;
              escape("unable to add the trait");
            }
        }

      // administrate the object.
      if (context.object.Administrate(
            context.object.meta.attributes,
            context.object.meta.owner.permissions) == elle::Status::Error)
        escape("unable to administrate the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    ///
    /// this method looks for the given name in the attributes and
    /// return the associated value.
    ///
    elle::Status        Attributes::Get(
                          gear::Object&                         context,
                          const elle::String&                   name,
                          nucleus::neutron::Trait*& trait)
    {

      // lookup in the attributes object.
      if (context.object.meta.attributes.Lookup(name,
                                                trait) == elle::Status::Error)
        escape("unable to lookup in the attributes");

      return elle::Status::Ok;
    }

    ///
    /// this method returns all the attributes.
    ///
    elle::Status        Attributes::Fetch(
                          gear::Object&                         context,
                          nucleus::neutron::Range<
                            nucleus::neutron::Trait>& range)
    {

      // consult the attributes.
      if (context.object.meta.attributes.Consult(
            std::numeric_limits<nucleus::neutron::Index>::min(),
            std::numeric_limits<nucleus::neutron::Size>::max(),
            range) == elle::Status::Error)
        escape("unable to fetch the attributes");

      return elle::Status::Ok;
    }

    ///
    /// this method removes an attribute.
    ///
    elle::Status        Attributes::Omit(
                          gear::Object&                         context,
                          const elle::String&                   name)
    {
      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // verify that the user can modify the attributes.
      if (context.rights.role != nucleus::neutron::RoleOwner)
        escape("the user does not seem to have the permission to update "
               "this object's attributes");

      // remove the trait associated with the given name.
      if (context.object.meta.attributes.Remove(name) == elle::Status::True)
        escape("unable to remove the trait");

      // administrate the object.
      if (context.object.Administrate(
            context.object.meta.attributes,
            context.object.meta.owner.permissions) == elle::Status::Error)
        escape("unable to administrate the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

  }
}
