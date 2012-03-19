//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon apr  5 15:13:38 2010]
//

#ifndef ETOILE_AUTOMATON_CONTENTS_HXX
#define ETOILE_AUTOMATON_CONTENTS_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Nucleus.hh>

#include <etoile/automaton/Rights.hh>
#include <etoile/automaton/Author.hh>
#include <etoile/automaton/Access.hh>

#include <etoile/depot/Depot.hh>

#include <hole/Hole.hh>

namespace etoile
{
  namespace automaton
  {

    ///
    /// this method opens a contents i.e loads if if presents or creates a
    /// new empty one.
    ///
    template <typename T>
    elle::Status        Contents::Open(
                          T&                                    context)
    {
      // if the contents is already opened, return.
      if (context.contents != NULL)
        return elle::StatusOk;

      // otherwise create a new contents according to the context's type.
      context.contents = new nucleus::Contents<typename T::C>;

      // check if there exists a contents. if so, load the block.
      if (context.object.data.contents != nucleus::Address::Null)
        {
          // load the block.
          if (depot::Depot::Pull(context.object.data.contents,
                                 nucleus::Version::Any,
                                 *context.contents) == elle::StatusError)
            escape("unable to load the contents");

          // determine the rights the current user has on this object.
          if (Rights::Determine(context) == elle::StatusError)
            escape("unable to determine the user's rights");

          // if the user has the permission to read, decrypt the content.
          if ((context.rights.permissions & nucleus::PermissionRead) ==
              nucleus::PermissionRead)
            {
              // decrypt the contents i.e the contents.
              if (context.contents->Decrypt(context.rights.key) ==
                  elle::StatusError)
                escape("unable to decrypt the contents");
            }
        }
      else
        {
          // otherwise, create an empty contents.
          if (context.contents->Create() == elle::StatusError)
            escape("unable to create the contents");
        }

      return elle::StatusOk;
    }

    ///
    /// this method destroys the contents by marking the blocks as dying.
    ///
    template <typename T>
    elle::Status        Contents::Destroy(
                          T&                                    context)
    {
      // if a block is referenced by the object, mark it as needing removal.
      if (context.object.data.contents != nucleus::Address::Null)
        {
          // mark the content block for removal.
          if (context.transcript.Wipe(context.object.data.contents) ==
              elle::StatusError)
            escape("unable to mark the content block for removal");
        }

      return elle::StatusOk;
    }

    ///
    /// this method computes the address of the contents, should it have
    /// changed, and update the object accordingly.
    ///
    /// if no modification has been performed, nothing is done.
    ///
    template <typename T>
    elle::Status        Contents::Close(
                          T&                                    context)
    {
      elle::SecretKey   key;
      nucleus::Size     size;

      //
      // first, check if the block has been modified i.e exists and is dirty.
      //
      {
        // if there is no loaded contents or accessible content, then there
        // is nothing to do.
        if (!((context.contents != NULL) &&
              (context.contents->content != NULL)))
          return elle::StatusOk;

        // if the contents has not changed, do nothing.
        if (context.contents->content->state == nucleus::StateClean)
          return elle::StatusOk;
      }

      // retrieve the contents's size.
      if (context.contents->content->Capacity(size) == elle::StatusError)
        escape("unable to retrieve the contents's size");

      //
      // at this point, the contents is known to have been modified.
      //

      // forge the author which will be attached to the modified object.
      if (Author::Forge(context) == elle::StatusError)
        escape("unable to forge an author");

      // modify the object according to the content.
      if (size == 0)
        {
          //
          // if the contents became empty after removals, the
          // object should no longer point to any contents block while
          // the old block should be deleted.
          //
          // however, since the object benefits from history i.e several
          // versions, removing the content block would render the previous
          // version inconsistent.
          //
          // therefore, the object is updated with a null content address.
          //
          // however, should the history functionality not be supported
          // by the network, the access block is marked for deletion.
          //

          // does the network support the history?
          if (hole::Hole::Descriptor.history == false)
            {
              // destroy the contents block.
              if (Contents::Destroy(context) == elle::StatusError)
                escape("unable to destroy the contents block");
            }

          // update the object with the null contents address.
          if (context.object.Update(
                context.author,
                nucleus::Address::Null,
                0,
                context.object.meta.access,
                context.object.meta.owner.token) == elle::StatusError)
            escape("unable to update the object");

          //
          // finally, since the data has changed (is now empty), the tokens
          // must be reinitialized to null.
          //

          // open the access.
          if (Access::Open(context) == elle::StatusError)
            escape("unable to open the access");

          // downgrade the access entries i.e set the tokens as null
          // since no content is present.
          if (Access::Downgrade(context) == elle::StatusError)
            escape("unable to downgrade the accesses");
        }
      else
        {
          //
          // otherwise, the new contents address needs to be computed and
          // the object updated accordingly.
          //
          // note that the previous content block is not removed since
          // objects benefit from the history i.e multiple versions; unless
          // the history support is not activated for this network.
          //

          nucleus::Address      address;

          // does the network support the history?
          if (hole::Hole::Descriptor.history == false)
            {
              // destroy the contents block.
              if (Contents::Destroy(context) == elle::StatusError)
                escape("unable to destroy the contents block");
            }

          // generate a secret key.
          if (key.Generate() == elle::StatusError) // XXX[should provide a len]
            escape("unable to generate the secret key");

          // encrypt the contents.
          if (context.contents->Encrypt(key) == elle::StatusError)
            escape("unable to encrypt the contents");

          // bind the contents i.e seal it by computing its address.
          if (context.contents->Bind(address) == elle::StatusError)
            escape("unable to bind the contents");

          // set the content as consistent.
          context.contents->content->state = nucleus::StateConsistent;

          // update the object.
          if (context.object.Update(
                context.author,
                address,
                size,
                context.object.meta.access,
                context.object.meta.owner.token) == elle::StatusError)
            escape("unable to update the object");

          // mark the block as needing to be stored.
          if (context.transcript.Push(address,
                                      context.contents) == elle::StatusError)
            escape("unable to record the object for storing");

          //
          // finally, since the data has been re-encrypted, the key must be
          // distributed to the users having been granted the read
          // permission.
          //

          // open the access.
          if (Access::Open(context) == elle::StatusError)
            escape("unable to open the access block");

          // upgrade the access entries with the new key.
          if (Access::Upgrade(context, key) == elle::StatusError)
            escape("unable to upgrade the accesses");
        }

      return elle::StatusOk;
    }

  }
}

#endif
