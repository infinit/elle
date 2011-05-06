//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/File.cc
//
// created       julien quintard   [fri aug 14 19:00:57 2009]
// updated       julien quintard   [thu may  5 16:17:37 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/File.hh>
#include <etoile/components/Rights.hh>
#include <etoile/components/Contents.hh>

#include <etoile/journal/Journal.hh>

#include <etoile/user/User.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a file object.
    ///
    elle::Status	File::Create(context::File*		context)
    {
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // allocate a new file object.
      context->object = new nucleus::Object;

      // create the irectory.
      if (context->object->Create(nucleus::GenreFile,
				  user->client->agent->K) == elle::StatusError)
	escape("unable to create the file object");

      // bind the object.
      if (context->object->Bind(context->address) == elle::StatusError)
	escape("unable to bind the object");

      leave();
    }

    ///
    /// this method loads the file object identified by the given
    /// address in the context.
    ///
    elle::Status	File::Load(context::File*		context,
				   const nucleus::Address&	address)
					
    {
      enter();

      // load the object.
      if (Object::Load(context, address) == elle::StatusError)
	escape("unable to load the object");

      // check that the object is a file.
      if (context->object->meta.genre != nucleus::GenreFile)
	escape("this object is not a file");

      leave();
    }

    ///
    /// this method write a specific region of the file.
    ///
    elle::Status	File::Write(context::File*		context,
				    const nucleus::Offset&	offset,
				    const elle::Region&		region)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the data.
      if (!(context->rights->record.permissions & nucleus::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // write the file.
      if (context->contents->content->Write(offset,
					    region) == elle::StatusError)
	escape("unable to write the file");

      leave();
    }

    ///
    /// this method returns a specific region of the file.
    ///
    elle::Status	File::Read(context::File*		context,
				   const nucleus::Offset&	offset,
				   const nucleus::Size&		size,
				   elle::Region&		region)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the data.
      if (!(context->rights->record.permissions & nucleus::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // read the file.
      if (context->contents->content->Read(offset,
					   size,
					   region) == elle::StatusError)
	escape("unable to read the file");

      leave();
    }

    ///
    /// this method adjusts the size of the file's contents.
    ///
    elle::Status	File::Adjust(context::File*		context,
				     const nucleus::Size&	size)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the data.
      if (!(context->rights->record.permissions & nucleus::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // adjust the size.
      if (context->contents->content->Adjust(size) == elle::StatusError)
	escape("unable to adjust the file size");

      leave();
    }

    ///
    /// this method discards the modifications applied onto the context.
    ///
    elle::Status	File::Discard(context::File*		context)
    {
      enter();

      // discard the object's modifications.
      if (Object::Discard(context) == elle::StatusError)
	escape("unable to discard the object modifications");

      leave();
    }

    ///
    /// this store the modifications applied onto the file context.
    ///
    elle::Status	File::Store(context::File*		context)
    {
      user::User*	user;

      enter();

      // close the catalog.
      if (Contents::Close(context) == elle::StatusError)
	escape("unable to close the contents");

      // store the object.
      if (Object::Store(context) == elle::StatusError)
	escape("unable to store the object");

      leave();
    }

    ///
    /// this method removes the object along with the blocks attached to it.
    ///
    elle::Status	File::Destroy(context::File*		context)
    {
      user::User*	user;
      nucleus::Size	size;

      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context->rights->role != nucleus::RoleOwner)
	escape("unable to destroy a not-owned object");

      // destroy the contents.
      if (Contents::Destroy(context) == elle::StatusError)
	escape("unable to destroy the contents");

      // destroy the object.
      if (Object::Destroy(context) == elle::StatusError)
	escape("unable to destroy the object");

      leave();
    }

  }
}
