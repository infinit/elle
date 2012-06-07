#include <nucleus/neutron/Data.hh>

#include <etoile/automaton/File.hh>
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
    /// this method creates a file object within the given context.
    ///
    elle::Status        File::Create(
                          gear::File&                           context)
    {
      nucleus::Address  address;

      // create the file.
      if (context.object.Create(
            nucleus::GenreFile,
            agent::Agent::Identity.pair.K) == elle::Status::Error)
        escape("unable to create the file object");

      // bind the object to its address i.e this will never changed.
      if (context.object.Bind(address) == elle::Status::Error)
        escape("unable to bind the object");

      // create the context's location with an initial version number.
      if (context.location.Create(address) == elle::Status::Error)
        escape("unable to create the location");

      // set the context's state.
      context.state = gear::Context::StateCreated;

      return elle::Status::Ok;
    }

    ///
    /// this method loads an existing file object identified by the
    /// given location.
    ///
    elle::Status        File::Load(
                          gear::File&                           context)

    {
      // return if the context has already been loaded.
      if (context.state != gear::Context::StateUnknown)
        return elle::Status::Ok;

      // load the object.
      if (Object::Load(context) == elle::Status::Error)
        escape("unable to fetch the object");

      // check that the object is a file.
      if (context.object.meta.genre != nucleus::GenreFile)
        escape("this object does not seem to be a file");

      // set the context's state.
      context.state = gear::Context::StateLoaded;

      return elle::Status::Ok;
    }

    ///
    /// this method writes a specific region of the file.
    ///
    elle::Status        File::Write(
                          gear::File&                           context,
                          const nucleus::Offset&                offset,
                          const elle::standalone::Region&                   region)
    {
      nucleus::Size     size;

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the write the data.
      if ((context.rights.permissions & nucleus::PermissionWrite) !=
          nucleus::PermissionWrite)
        escape("the user does not seem to have the permission to write "
               "this file");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
        escape("the user does not seem to be able to operate on this "
               "file");

      // write the file.
      if (context.contents->content->Write(offset,
                                           region) == elle::Status::Error)
        escape("unable to write the file");

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::Status::Error)
        escape("unable to retrieve the contents's size");

      // update the object.
      if (context.object.Update(
            context.object.author,
            context.object.data.contents,
            size,
            context.object.meta.access,
            context.object.meta.owner.token) == elle::Status::Error)
        escape("unable to update the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    ///
    /// this method returns a specific region of the file.
    ///
    elle::Status        File::Read(
                          gear::File&                           context,
                          const nucleus::Offset&                offset,
                          const nucleus::Size&                  size,
                          elle::standalone::Region&                         region)
    {
      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the read the data.
      if ((context.rights.permissions & nucleus::PermissionRead) !=
          nucleus::PermissionRead)
        escape("the user does not seem to have the permission to read "
               "this file");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
        escape("the user does not seem to be able to operate on this "
               "file");

      // read the file.
      if (context.contents->content->Read(offset,
                                          size,
                                          region) == elle::Status::Error)
        escape("unable to read the file");

      return elle::Status::Ok;
    }

    ///
    /// this method adjusts the size of the file's contents.
    ///
    elle::Status        File::Adjust(
                          gear::File&                           context,
                          const nucleus::Size&                  length)
    {
      nucleus::Size     size;

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the adjust the file.
      if ((context.rights.permissions & nucleus::PermissionWrite) !=
          nucleus::PermissionWrite)
        escape("the user does not seem to have the permission to adjust "
               "this file");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
        escape("the user does not seem to be able to operate on this "
               "file");

      // adjust the data size.
      if (context.contents->content->Adjust(length) == elle::Status::Error)
        escape("unable to adjust the file size");

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::Status::Error)
        escape("unable to retrieve the contents's size");

      // update the object.
      if (context.object.Update(
            context.object.author,
            context.object.data.contents,
            size,
            context.object.meta.access,
            context.object.meta.owner.token) == elle::Status::Error)
        escape("unable to update the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    ///
    /// this method is called whenever the context is being closed without
    /// any modification having been performed.
    ///
    elle::Status        File::Discard(
                          gear::File&                           context)
    {
      // set the context's state.
      context.state = gear::Context::StateDiscarded;

      return elle::Status::Ok;
    }

    ///
    /// this method destroys the file by marking all the blocks
    /// as dying for future removal.
    ///
    elle::Status        File::Destroy(
                          gear::File&                           context)
    {
      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context.rights.role != nucleus::RoleOwner)
        escape("the user does not seem to have the permission to destroy "
               "this file");

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
    /// this method terminates the automaton by making the whole file
    /// consistent according to the set of modifications having been performed.
    ///
    elle::Status        File::Store(
                          gear::File&                           context)
    {
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
