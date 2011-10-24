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
    elle::Status	File::Create(
			  gear::File&				context)
    {
      nucleus::Address	address;

      enter();

      // create the file.
      if (context.object.Create(
	    nucleus::GenreFile,
	    agent::Agent::Identity.pair.K) == elle::StatusError)
	escape("unable to create the file object");

      // bind the object to its address i.e this will never changed.
      if (context.object.Bind(address) == elle::StatusError)
	escape("unable to bind the object");

      // create the context's location with an initial version number.
      if (context.location.Create(address) == elle::StatusError)
	escape("unable to create the location");

      // set the context's state.
      context.state = gear::Context::StateInitialized;

      leave();
    }

    ///
    /// this method loads an existing file object identified by the
    /// given location.
    ///
    elle::Status	File::Load(
			  gear::File&				context)
					
    {
      enter();

      // load the object.
      if (Object::Load(context) == elle::StatusError)
	escape("unable to fetch the object");

      // check that the object is a file.
      if (context.object.meta.genre != nucleus::GenreFile)
	escape("this object does not seem to be a file");

      // set the context's state.
      context.state = gear::Context::StateInitialized;

      leave();
    }

    ///
    /// this method writes a specific region of the file.
    ///
    elle::Status	File::Write(
			  gear::File&				context,
			  const nucleus::Offset&		offset,
			  const elle::Region&			region)
    {
      nucleus::Size	size;

      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the data.
      if (!(context.rights.permissions & nucleus::PermissionWrite))
	escape("the user does not seem to have the permission to write "
	       "this file");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
	escape("the user does not seem to be able to operate on this "
	       "directory");

      // write the file.
      if (context.contents->content->Write(offset,
					   region) == elle::StatusError)
	escape("unable to write the file");

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::StatusError)
	escape("unable to retrieve the contents's size");

      // update the object data section.
      if (context.object.Update(
	    context.object.author,
	    context.object.data.contents,
	    size) == elle::StatusError)
	escape("unable to update the object data section");

      // set the context's state.
      context.state = gear::Context::StateModified;

      leave();
    }

    ///
    /// this method returns a specific region of the file.
    ///
    elle::Status	File::Read(
			  gear::File&				context,
			  const nucleus::Offset&		offset,
			  const nucleus::Size&			size,
			  elle::Region&				region)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the data.
      if (!(context.rights.permissions & nucleus::PermissionRead))
	escape("the user does not seem to have the permission to read "
	       "this file");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
	escape("the user does not seem to be able to operate on this "
	       "directory");

      // read the file.
      if (context.contents->content->Read(offset,
					  size,
					  region) == elle::StatusError)
	escape("unable to read the file");

      leave();
    }

    ///
    /// this method adjusts the size of the file's contents.
    ///
    elle::Status	File::Adjust(
			  gear::File&				context,
			  const nucleus::Size&			length)
    {
      nucleus::Size	size;

      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the adjust the file.
      if (!(context.rights.permissions & nucleus::PermissionRead))
	escape("the user does not seem to have the permission to adjust "
	       "this file");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
	escape("the user does not seem to be able to operate on this "
	       "directory");

      // adjust the data size.
      if (context.contents->content->Adjust(length) == elle::StatusError)
	escape("unable to adjust the file size");

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::StatusError)
	escape("unable to retrieve the contents's size");

      // update the object data section.
      if (context.object.Update(
	    context.object.author,
	    context.object.data.contents,
	    size) == elle::StatusError)
	escape("unable to update the object data section");

      // set the context's state.
      context.state = gear::Context::StateModified;

      leave();
    }

    ///
    /// this method is called whenever the context is being closed without
    /// any modification having been performed.
    ///
    elle::Status	File::Discard(
			  gear::File&				context)
    {
      enter();

      // set the context's state.
      context.state = gear::Context::StateDiscarded;

      leave();
    }

    ///
    /// this method destroys the file by marking all the blocks
    /// as dying for future removal.
    ///
    elle::Status	File::Destroy(
			  gear::File&				context)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context.rights.role != nucleus::RoleOwner)
	escape("the user does not seem to have the permission to destroy "
	       "this file");

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

      leave();
    }

    ///
    /// this method terminates the automaton by making the whole file
    /// consistent according to the set of modifications having been performed.
    ///
    elle::Status	File::Store(
			  gear::File&				context)
    {
      enter();

      // close the contents.
      if (Contents::Close(context) == elle::StatusError)
	escape("unable to close the contents");

      // store the object-related information.
      if (Object::Store(context) == elle::StatusError)
	escape("unable to store the object");

      // set the context's state.
      context.state = gear::Context::StateStored;

      leave();
    }

  }
}
