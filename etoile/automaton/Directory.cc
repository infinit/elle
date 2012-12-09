#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Entry.hh>

#include <etoile/automaton/Directory.hh>
#include <etoile/automaton/Object.hh>
#include <etoile/automaton/Contents.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/gear/Directory.hh>

#include <agent/Agent.hh>

#include <elle/print.hh>

ELLE_LOG_COMPONENT("infinit.etoile.automaton.Directory");

namespace etoile
{
  namespace automaton
  {
    ///
    /// this method creates a directory object within the given context.
    ///
    elle::Status        Directory::Create(
                          gear::Directory&                      context)
    {
      ELLE_TRACE_FUNCTION(context);

      context.object =
        new nucleus::neutron::Object(nucleus::proton::Network(Infinit::Network),
                                     agent::Agent::Identity.pair.K(),
                                     nucleus::neutron::Genre::directory);

      nucleus::proton::Address address(context.object->bind());

      // create the context's location with an initial revision number.
      context.location =
        nucleus::proton::Location(address, context.object->revision());

      // set the context's state.
      context.state = gear::Context::StateCreated;

      return elle::Status::Ok;
    }

    ///
    /// this method loads an existing directory object identified by the
    /// given location.
    ///
    elle::Status        Directory::Load(
                          gear::Directory&                      context)
    {
      ELLE_TRACE_FUNCTION(context);

      // return if the context has already been loaded.
      if (context.state != gear::Context::StateUnknown)
        return elle::Status::Ok;

      // load the object.
      if (Object::Load(context) == elle::Status::Error)
        escape("unable to fetch the object");

      // check that the object is a directory.
      if (context.object->genre() != nucleus::neutron::Genre::directory)
        escape("this object does not seem to be a directory");

      // set the context's state.
      context.state = gear::Context::StateLoaded;

      return elle::Status::Ok;
    }

    ///
    /// this method adds an entry to the directory.
    ///
    elle::Status        Directory::Add(
                          gear::Directory&                      context,
                          const path::Slice&                    name,
                          const nucleus::proton::Address& address)
    {
      ELLE_TRACE_FUNCTION(context, name, address);

      nucleus::neutron::Size size;

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the write the catalog.
      if ((context.rights.permissions & nucleus::neutron::permissions::write) !=
          nucleus::neutron::permissions::write)
        escape("the user does not seem to have the permission to write "
               "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.porcupine == nullptr)
        escape("the user does not seem to be able to operate on this "
               "directory");

      // Retrieve a door on the catalog.
      nucleus::proton::Door<nucleus::neutron::Catalog> door{
        context.porcupine->lookup(name)};

      door.open();

      // Add the entry to the catalog.
      door().insert(new nucleus::neutron::Entry{name, address});

      door.close();

      // Update the porcupine.
      context.porcupine->update(name);

      // update the object.
      if (context.object->Update(
            context.object->author(),
            context.object->contents(),
            context.porcupine->size(),
            context.object->access(),
            context.object->owner_token()) == elle::Status::Error)
        escape("unable to update the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    ///
    /// this method returns the directory entry associated with the
    /// given name.
    ///
    elle::Status        Directory::Lookup(
                          gear::Directory&                      context,
                          const path::Slice&                    name,
                          nucleus::neutron::Entry const*& entry)
    {
      ELLE_TRACE_FUNCTION(context, name);

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::neutron::permissions::read) !=
          nucleus::neutron::permissions::read)
        escape("%s", (
            "the user does not seem to have the permission to read "
            "this directory (permissions=" +
            elle::sprint(context.rights.permissions) + ")"
        ).c_str());

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.porcupine == nullptr)
        escape("the user does not seem to be able to operate on this "
               "directory");

      // Retrieve a door on the catalog.
      nucleus::proton::Door<nucleus::neutron::Catalog> door{
        context.porcupine->lookup(name)};

      door.open();

      // Look up the entry.
      // XXX[we take the address of the reference: wrong]
      entry = &(door().locate(name));

      door.close();

      return elle::Status::Ok;
    }

    ///
    /// this method returns a subset of the directory entries.
    ///
    /// the subset is represented by the arguments _index_ and _size_.
    ///
    elle::Status        Directory::Consult(
                          gear::Directory&                      context,
                          const nucleus::neutron::Index& index,
                          const nucleus::neutron::Size& size,
                          nucleus::neutron::Range<
                            nucleus::neutron::Entry>& range)
    {
      ELLE_TRACE_FUNCTION(context, index, size);

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::neutron::permissions::read) !=
          nucleus::neutron::permissions::read)
        escape("the user does not seem to have the permission to read "
               "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.porcupine == nullptr)
        escape("the user does not seem to be able to operate on this "
               "directory");

      // Seek the catalog responsible for the given index.
      auto _index = static_cast<nucleus::proton::Capacity>(index);
      auto _size = size;

      while (_size > 0)
        {
          auto pair = context.porcupine->seek(_index);
          auto& door = pair.first;
          auto& base = pair.second;

          door.open();

          auto start = _index - base;
          auto length = _size > door().size() - start ?
            door().size() - start :
            _size;

          // Retrieve the directory entries falling in the requested
          // range [index, index + size[.
          nucleus::neutron::Range<nucleus::neutron::Entry> subrange{
            door().consult(start, length)};

          door.close();

          // Inject/merge the retrieved entries into the main range.
          range.add(subrange);

          // Update the variables _index and _size.
          _index += length;
          _size -= length;
        }

      ELLE_ASSERT(range.size() == size);

      return elle::Status::Ok;
    }

    ///
    /// this method renames a directory entry.
    ///
    elle::Status        Directory::Rename(
                          gear::Directory&                      context,
                          const path::Slice&                    from,
                          const path::Slice&                    to)
    {
      ELLE_TRACE_FUNCTION(context, from, to);

      nucleus::neutron::Size size;

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::neutron::permissions::write) !=
          nucleus::neutron::permissions::write)
        escape("the user does not seem to have the permission to write "
               "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.porcupine == nullptr)
        escape("the user does not seem to be able to operate on this "
               "directory");

      // Perform an optimization here: if the porcupine is evolving in the value
      // or block strategy, just rename the entry within the only present
      // catalog.
      //
      // Otherwise, the catalogs have to be located in the tree.
      switch (context.porcupine->strategy())
        {
        case nucleus::proton::Strategy::none:
          throw elle::Exception("invalid 'none' strategy");
        case nucleus::proton::Strategy::value:
        case nucleus::proton::Strategy::block:
          {
            nucleus::proton::Door<nucleus::neutron::Catalog> door{
              context.porcupine->lookup(from)};

            door.open();

            // Directly rename the entry in the catalog block.
            door().rename(from, to);

            door.close();

            break;
          }
        case nucleus::proton::Strategy::tree:
          {
            // Retrieve a doors on the catalogs containing the _from_ entry
            // and the one responsible for the range in which the _to_ lies.
            nucleus::proton::Door<nucleus::neutron::Catalog> door_from{
              context.porcupine->lookup(from)};
            nucleus::proton::Door<nucleus::neutron::Catalog> door_to{
              context.porcupine->lookup(to)};

            if (door_from == door_to)
              {
                door_from.open();

                // In this case, the renamed entry will be located in the
                // same catalog block. Therefore, the entry is directly
                // renamed within the catalog.
                door_from().rename(from, to);

                door_from.close();

                // Note however that the major key or else may have changed.
                //
                // Thus, the porcupine needs to be updated. However, a single
                // call is required because we know both keys lies in the same
                // catalog block.
                //
                // Caution is however taken by using the highest key for the
                // updated call. We never know how the tree may be optimized
                // to perform special operations with the mayor key (which
                // the highest of both may actually be).
                context.porcupine->update(from > to ? from : to);
              }
            else
              {
                // Otherwise, the entry must be relocated in the appropriate
                // block i.e 'removed and re-inserted'.

                door_from.open();

                // Take the entry out of the original catalog block.
                std::shared_ptr<nucleus::neutron::Entry> entry =
                  door_from().takeout(from);

                door_from.close();
                door_to.open();

                // Re-insert the entry in its new catalog block so as to
                // keep the tree sorting consistent.
                door_to().insert(entry);

                door_to.close();

                // Update the porcupine twice because two blocks have been
                // modified.
                //
                // Note that both optimizations are requested at the end and
                // not after each operation: takeout/insert.
                context.porcupine->update(from);
                context.porcupine->update(to);
              }

            break;
          }
        default:
          throw elle::Exception("unknown strategy '%s'",
                                context.porcupine->strategy());
        }

      // update the object though renaming an entry may not impact
      // the object's data size.
      if (context.object->Update(
            context.object->author(),
            context.object->contents(),
            context.porcupine->size(),
            context.object->access(),
            context.object->owner_token()) == elle::Status::Error)
        escape("unable to update the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    ///
    /// this method removes a directory entry.
    ///
    elle::Status        Directory::Remove(
                          gear::Directory&                      context,
                          const path::Slice&                    name)
    {
      ELLE_TRACE_FUNCTION(context, name);

      nucleus::neutron::Size size;

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::neutron::permissions::write) !=
          nucleus::neutron::permissions::write)
        escape("the user does not seem to have the permission to write "
               "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.porcupine == nullptr)
        escape("the user does not seem to be able to operate on this "
               "directory");

      // Retrieve a door on the catalog.
      nucleus::proton::Door<nucleus::neutron::Catalog> door{
        context.porcupine->lookup(name)};

      door.open();

      // Remove the entry from the catalog.
      door().erase(name);

      door.close();

      // Update the porcupine.
      context.porcupine->update(name);

      // update the object.
      if (context.object->Update(
            context.object->author(),
            context.object->contents(),
            context.porcupine->size(),
            context.object->access(),
            context.object->owner_token()) == elle::Status::Error)
        escape("unable to update the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    ///
    /// this method is called whenever the context is being closed without
    /// any modification having been performed.
    ///
    elle::Status        Directory::Discard(
                          gear::Directory&                      context)
    {
      ELLE_TRACE_FUNCTION(context);

      // discard the object-related information.
      if (Object::Destroy(context) == elle::Status::Error)
        escape("unable to destroy the object");

      // set the context's state.
      context.state = gear::Context::StateDiscarded;

      return elle::Status::Ok;
    }

    ///
    /// this method destroys the directory by marking all the blocks
    /// as dying for future removal.
    ///
    elle::Status        Directory::Destroy(
                          gear::Directory&                      context)
    {
      ELLE_TRACE_FUNCTION(context);

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context.rights.role != nucleus::neutron::Object::RoleOwner)
        escape("the user does not seem to have the permission to destroy "
               "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // destroy the contents.
      if (Contents::Destroy(context) == elle::Status::Error)
        escape("unable to destroy the contents");

      // destroy the object-related information.
      if (Object::Destroy(context) == elle::Status::Error)
        escape("unable to destroy the object");

      // set the context's state.
      context.state = gear::Context::StateDestroyed;

      return elle::Status::Ok;
    }

    ///
    /// this method terminates the automaton by making the whole directory
    /// consistent according to the set of modifications having been performed.
    ///
    elle::Status
    Directory::Store(gear::Directory& context)
    {
      ELLE_TRACE_FUNCTION(context);

      // close the contents.
      if (Contents::Close(context) == elle::Status::Error)
        escape("unable to close the contents");

      // store the object-related information.
      if (Object::Store(context) == elle::Status::Error)
        escape("unable to store the object");

      // set the context's state.
      context.state = gear::Context::StateStored;

      return elle::Status::Ok;
    }

  }
}
