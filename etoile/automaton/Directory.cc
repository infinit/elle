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
      ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, context);
      nucleus::proton::Address address;

      context.object = new nucleus::neutron::Object;

      // create the directory.
      if (context.object->Create(
            nucleus::neutron::GenreDirectory,
            agent::Agent::Identity.pair.K) == elle::Status::Error)
        escape("unable to create the directory object");

      // bind the object to its address i.e this will never changed.
      if (context.object->Bind(address) == elle::Status::Error)
        escape("unable to bind the object");

      // create the context's location with an initial version number.
      if (context.location.Create(address,
                                  context.object->version) == elle::Status::Error)
        escape("unable to create the location");

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
      ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, context);

      // return if the context has already been loaded.
      if (context.state != gear::Context::StateUnknown)
        return elle::Status::Ok;

      // load the object.
      if (Object::Load(context) == elle::Status::Error)
        escape("unable to fetch the object");

      // check that the object is a directory.
      if (context.object->genre() != nucleus::neutron::GenreDirectory)
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
      ELLE_TRACE_SCOPE("%s(%s, %s, %s)", __FUNCTION__, context, name, address);
      nucleus::neutron::Size size;

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the write the catalog.
      if ((context.rights.permissions & nucleus::neutron::PermissionWrite) !=
          nucleus::neutron::PermissionWrite)
        escape("the user does not seem to have the permission to write "
               "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // allocate a new directory entry.
      std::unique_ptr<nucleus::neutron::Entry> entry(
        new nucleus::neutron::Entry(name, address));

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == nullptr)
        escape("the user does not seem to be able to operate on this "
               "directory");

      // add the entry in the directory.
      if (context.contents->content->Add(entry.get()) == elle::Status::Error)
        escape("unable to add the entry in the directory");

      // stop tracking since the entry has been properly added to the catalog.
      entry.release();

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::Status::Error)
        escape("unable to retrieve the contents's size");

      // update the object.
      if (context.object->Update(
            context.object->author(),
            context.object->contents(),
            size,
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
      ELLE_TRACE_SCOPE("%s(%s, %s)", __FUNCTION__, context, name);

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::neutron::PermissionRead) !=
          nucleus::neutron::PermissionRead)
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
      if (context.contents->content == nullptr)
        escape("the user does not seem to be able to operate on this "
               "directory");

      // look up the entry.
      if (context.contents->content->Lookup(name,
                                            entry) == elle::Status::Error)
        escape("unable to find the entry in the directory");

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
      ELLE_TRACE_SCOPE("%s(%s, %s, %s, %s)", __FUNCTION__, context, index, size, range);

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::neutron::PermissionRead) !=
          nucleus::neutron::PermissionRead)
        escape("the user does not seem to have the permission to read "
               "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == nullptr)
        escape("the user does not seem to be able to operate on this "
               "directory");

      // consult the directory catalog.
      if (context.contents->content->Consult(index,
                                             size,
                                             range) == elle::Status::Error)
        escape("unable to consult the directory");

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
      ELLE_TRACE_SCOPE("%s(%s, %s, %s)", __FUNCTION__, context, from, to);

      nucleus::neutron::Size size;

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::neutron::PermissionWrite) !=
          nucleus::neutron::PermissionWrite)
        escape("the user does not seem to have the permission to write "
               "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == nullptr)
        escape("the user does not seem to be able to operate on this "
               "directory");

      // rename the entry.
      if (context.contents->content->Rename(from, to) == elle::Status::Error)
        escape("unable to rename the directory's entry");

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::Status::Error)
        escape("unable to retrieve the contents's size");

      // update the object though renaming an entry may not impact
      // the object's data size.
      if (context.object->Update(
            context.object->author(),
            context.object->contents(),
            size,
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
      ELLE_TRACE_SCOPE("%s(%s, %s)", __FUNCTION__, context, name);

      nucleus::neutron::Size size;

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::neutron::PermissionWrite) !=
          nucleus::neutron::PermissionWrite)
        escape("the user does not seem to have the permission to write "
               "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == nullptr)
        escape("the user does not seem to be able to operate on this "
               "directory");

      // remove the entry.
      if (context.contents->content->Remove(name) == elle::Status::Error)
        escape("unable to remove the directory's entry");

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::Status::Error)
        escape("unable to retrieve the contents's size");

      // update the object.
      if (context.object->Update(
            context.object->author(),
            context.object->contents(),
            size,
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
      ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, context);

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
      ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, context);

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
    elle::Status        Directory::Store(
                          gear::Directory&                      context)
    {
      ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, context);

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
