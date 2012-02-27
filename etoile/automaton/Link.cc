//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 19:00:57 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/automaton/Link.hh>
#include <etoile/automaton/Object.hh>
#include <etoile/automaton/Contents.hh>
#include <etoile/automaton/Rights.hh>

#include <agent/Agent.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a link object within the given context.
    ///
    elle::Status        Link::Create(
                          gear::Link&                           context)
    {
      nucleus::Address  address;

      // create the link.
      if (context.object.Create(
            nucleus::GenreLink,
            agent::Agent::Identity.pair.K) == elle::StatusError)
        escape("unable to create the link object");

      // bind the object to its address i.e this will never changed.
      if (context.object.Bind(address) == elle::StatusError)
        escape("unable to bind the object");

      // create the context's location with an initial version number.
      if (context.location.Create(address) == elle::StatusError)
        escape("unable to create the location");

      // set the context's state.
      context.state = gear::Context::StateCreated;

      return elle::StatusOk;
    }

    ///
    /// this method loads an existing link object identified by the
    /// given location.
    ///
    elle::Status        Link::Load(
                          gear::Link&                           context)
    {
      // return if the context has already been loaded.
      if (context.state != gear::Context::StateUnknown)
        return elle::StatusOk;

      // load the object.
      if (Object::Load(context) == elle::StatusError)
        escape("unable to fetch the object");

      // check that the object is a link.
      if (context.object.meta.genre != nucleus::GenreLink)
        escape("this object does not seem to be a link");

      // set the context's state.
      context.state = gear::Context::StateLoaded;

      return elle::StatusOk;
    }

    ///
    /// this method binds a new target to the link.
    ///
    elle::Status        Link::Bind(
                          gear::Link&                           context,
                          const path::Way&                      way)
    {
      nucleus::Size     size;

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
        escape("unable to determine the rights");

      // check if the current user has the right the bind the link.
      if ((context.rights.permissions & nucleus::PermissionWrite) !=
          nucleus::PermissionWrite)
        escape("the user does not seem to have the permission to bind "
               "this link");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
        escape("the user does not seem to be able to operate on this "
               "link");

      // bind the link.
      if (context.contents->content->Bind(way.path) == elle::StatusError)
        escape("unable to bind the link");

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::StatusError)
        escape("unable to retrieve the contents's size");

      // update the object.
      if (context.object.Update(
            context.object.author,
            context.object.data.contents,
            size,
            context.object.meta.access,
            context.object.meta.owner.token) == elle::StatusError)
        escape("unable to update the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::StatusOk;
    }

    ///
    /// this method returns the way associated with this link.
    ///
    elle::Status        Link::Resolve(
                          gear::Link&                           context,
                          path::Way&                            way)
    {
      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
        escape("unable to determine the rights");

      // check if the current user has the right the resolve the link..
      if ((context.rights.permissions & nucleus::PermissionRead) !=
          nucleus::PermissionRead)
        escape("the user does not seem to have the permission to resolve "
               "this link");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
        escape("the user does not seem to be able to operate on this "
               "link");

      // resolve the link.
      if (context.contents->content->Resolve(way.path) == elle::StatusError)
        escape("unable to resolve the link");

      return elle::StatusOk;
    }

    ///
    /// this method is called whenever the context is being closed without
    /// any modification having been performed.
    ///
    elle::Status        Link::Discard(
                          gear::Link&                           context)
    {
      // set the context's state.
      context.state = gear::Context::StateDiscarded;

      return elle::StatusOk;
    }

    ///
    /// this method destroys the link by marking all the blocks
    /// as dying for future removal.
    ///
    elle::Status        Link::Destroy(
                          gear::Link&                           context)
    {
      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
        escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context.rights.role != nucleus::RoleOwner)
        escape("the user does not seem to have the permission to destroy "
               "this link");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
        escape("unable to open the contents");

      // destroy the contents.
      if (Contents::Destroy(context) == elle::StatusError)
        escape("unable to destroy the contents");

      // destroy the object-related information.
      if (Object::Destroy(context) == elle::StatusError)
        escape("unable to destroy the object");

      // set the context's state.
      context.state = gear::Context::StateDestroyed;

      return elle::StatusOk;
    }

    ///
    /// this method terminates the automaton by making the whole link
    /// consistent according to the set of modifications having been performed.
    ///
    elle::Status        Link::Store(
                          gear::Link&                           context)
    {
      // close the contents.
      if (Contents::Close(context) == elle::StatusError)
        escape("unable to close the contents");

      // store the object-related information.
      if (Object::Store(context) == elle::StatusError)
        escape("unable to store the object");

      // set the context's state.
      context.state = gear::Context::StateStored;

      return elle::StatusOk;
    }

  }
}
