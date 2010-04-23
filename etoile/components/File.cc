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
// updated       julien quintard   [thu apr 22 11:10:01 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/File.hh>

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
    Status		File::Create(context::File*		context)
    {
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // allocate a new file object.
      context->object = new kernel::Object;

      // create the irectory.
      if (context->object->Create(kernel::GenreFile,
				  user->client->agent->K) == StatusError)
	escape("unable to create the file object");

      // bind the object.
      if (context->object->Bind() == StatusError)
	escape("unable to bind the object");

      // set the address.
      context->address = context->object->address;

      leave();
    }

    ///
    /// this method loads the file object identified by the given
    /// address in the context.
    ///
    Status		File::Load(context::File*		context,
				   const hole::Address&		address)
					
    {
      enter();

      // load the object.
      if (Object::Load(context, address) == StatusError)
	escape("unable to load the object");

      // check that the object is a file.
      if (context->object->meta.genre != kernel::GenreFile)
	escape("this object is not a file");

      leave();
    }

    ///
    /// this method write a specific region of the file.
    ///
    Status		File::Write(context::File*		context,
				    const kernel::Offset&	offset,
				    const Region&		region)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the data.
      if (!(context->rights->record.permissions & kernel::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == StatusError)
	escape("unable to open the contents");

      // write the file.
      if (context->contents->content->Write(offset, region) == StatusError)
	escape("unable to write the file");

      leave();
    }

    ///
    /// this method returns a specific region of the file.
    ///
    Status		File::Read(context::File*		context,
				   const kernel::Offset&	offset,
				   const kernel::Size&		size,
				   Region&			region)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the data.
      if (!(context->rights->record.permissions & kernel::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == StatusError)
	escape("unable to open the contents");

      // read the file.
      if (context->contents->content->Read(offset,
					   size,
					   region) == StatusError)
	escape("unable to read the file");

      leave();
    }

    ///
    /// this method adjusts the size of the file's contents.
    ///
    Status		File::Adjust(context::File*		context,
				     const kernel::Size&	size)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the data.
      if (!(context->rights->record.permissions & kernel::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == StatusError)
	escape("unable to open the contents");

      // adjust the size.
      if (context->contents->content->Adjust(size) == StatusError)
	escape("unable to adjust the file size");

      leave();
    }

    ///
    /// this method discards the modifications applied onto the context.
    ///
    Status		File::Discard(context::File*		context)
    {
      enter();

      // discard the object's modifications.
      if (Object::Discard(context) == StatusError)
	escape("unable to discard the object modifications");

      leave();
    }

    ///
    /// this store the modifications applied onto the file context.
    ///
    Status		File::Store(context::File*		context)
    {
      user::User*	user;

      enter();

      // close the catalog.
      if (Contents::Close(context) == StatusError)
	escape("unable to close the contents");

      // store the object.
      if (Object::Store(context) == StatusError)
	escape("unable to store the object");

      leave();
    }

    ///
    /// this method removes the object along with the blocks attached to it.
    ///
    Status		File::Destroy(context::File*		context)
    {
      user::User*	user;
      kernel::Size	size;

      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context->rights->role != kernel::RoleOwner)
	escape("unable to destroy a not-owned object");

      // destroy the contents.
      if (Contents::Destroy(context) == StatusError)
	escape("unable to destroy the contents");

      // destroy the object.
      if (Object::Destroy(context) == StatusError)
	escape("unable to destroy the object");

      leave();
    }

  }
}
