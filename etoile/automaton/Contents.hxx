#ifndef ETOILE_AUTOMATON_CONTENTS_HXX
# define ETOILE_AUTOMATON_CONTENTS_HXX

# include <nucleus/proton/Version.hh>
# include <nucleus/proton/Address.hh>
# include <nucleus/proton/State.hh>
# include <nucleus/proton/Contents.hh>
# include <nucleus/neutron/Permissions.hh>
# include <nucleus/neutron/Size.hh>

# include <etoile/automaton/Rights.hh>
# include <etoile/automaton/Author.hh>
# include <etoile/automaton/Access.hh>

# include <etoile/depot/Depot.hh>

# include <hole/Hole.hh>

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
      ELLE_LOG_COMPONENT("infinit.etoile.automaton.Contents");

      ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, context);

      // if the contents is already opened, return.
      if (context.contents != nullptr)
        return elle::Status::Ok;

      // check if there exists a contents. if so, load the block.
      if (context.object->contents() != nucleus::proton::Address::Null)
        {
          // load the block.
          try
            {
              // XXX[the context should make use of unique_ptr instead
              //     of releasing here.]
              context.contents =
                depot::Depot::pull<nucleus::proton::Contents<typename T::C>>(
                context.object->contents()).release();
            }
          catch (std::runtime_error& e)
            {
              escape("unable to retrieve the contents block: %s", e.what());
            }

          // determine the rights the current user has on this object.
          if (Rights::Determine(context) == elle::Status::Error)
            escape("unable to determine the user's rights");

          // if the user has the permission to read, decrypt the content.
          if ((context.rights.permissions & nucleus::neutron::PermissionRead) ==
              nucleus::neutron::PermissionRead)
            {
              // decrypt the contents i.e the contents.
              if (context.contents->Decrypt(context.rights.key) ==
                  elle::Status::Error)
                escape("unable to decrypt the contents");
            }
        }
      else
        {
          // otherwise create a new contents according to the context's type.
          context.contents = new nucleus::proton::Contents<typename T::C>;

          // otherwise, create an empty contents.
          if (context.contents->Create() == elle::Status::Error)
            escape("unable to create the contents");
        }

      assert(context.contents);

      return elle::Status::Ok;
    }

    ///
    /// this method destroys the contents by marking the blocks as dying.
    ///
    template <typename T>
    elle::Status        Contents::Destroy(
                          T&                                    context)
    {
      ELLE_LOG_COMPONENT("infinit.etoile.automaton.Contents");

      ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, context);

      // if a block is referenced by the object, mark it as needing removal.
      if (context.object->contents() != nucleus::proton::Address::Null)
        {
          ELLE_TRACE("record the Contents block '%s' for removal",
                     context.object->contents());

          // mark the content block for removal.
          if (context.transcript.Wipe(
                context.object->contents()) == elle::Status::Error)
            escape("unable to mark the content block for removal");
        }

      return elle::Status::Ok;
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
      ELLE_LOG_COMPONENT("infinit.etoile.automaton.Contents");

      elle::cryptography::SecretKey   key;
      nucleus::neutron::Size     size;

      ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, context);

      //
      // first, check if the block has been modified i.e exists and is dirty.
      //
      {
        // if there is no loaded contents or accessible content, then there
        // is nothing to do.
        if (!((context.contents != nullptr) &&
              (context.contents->content != nullptr)))
          return elle::Status::Ok;

        // if the contents has not changed, do nothing.
        if (context.contents->state == nucleus::proton::StateClean)
          return elle::Status::Ok;
      }

      ELLE_TRACE("the Contents block seems to have been modified");

      // retrieve the contents's size.
      if (context.contents->content->Capacity(size) == elle::Status::Error)
        escape("unable to retrieve the contents's size");

      //
      // at this point, the contents is known to have been modified.
      //

      // forge the author which will be attached to the modified object.
      if (Author::Forge(context) == elle::Status::Error)
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

          ELLE_TRACE("the Contents block is empty");

          // does the network support the history?
          if (hole::Hole::Descriptor.meta().history() == false)
            {
              // destroy the contents block.
              if (Contents::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the contents block");
            }

          ELLE_TRACE("update the object with a null Contents address")
            {
              // update the object with the null contents address.
              if (context.object->Update(
                    context.author,
                    nucleus::proton::Address::Null,
                    0,
                    context.object->access(),
                    context.object->owner_token()) == elle::Status::Error)
                escape("unable to update the object");
            }

          //
          // finally, since the data has changed (is now empty), the tokens
          // must be reinitialized to null.
          //

          ELLE_TRACE("open the Access block")
            {
              // open the access.
              if (Access::Open(context) == elle::Status::Error)
                escape("unable to open the access");
            }

          ELLE_TRACE("downgrade the Access record")
            {
              // downgrade the access entries i.e set the tokens as null
              // since no content is present.
              if (Access::Downgrade(context) == elle::Status::Error)
                escape("unable to downgrade the accesses");
            }
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
          nucleus::proton::Address address;

          ELLE_TRACE("the Contents block is _not_ empty");

          // does the network support the history?
          if (hole::Hole::Descriptor.meta().history() == false)
            {
              // destroy the contents block.
              if (Contents::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the contents block");
            }

          // generate a secret key.
          if (key.Generate() == elle::Status::Error) // XXX[should provide a len]
            escape("unable to generate the secret key");

          // encrypt the contents.
          if (context.contents->Encrypt(key) == elle::Status::Error)
            escape("unable to encrypt the contents");

          // bind the contents i.e seal it by computing its address.
          if (context.contents->Bind(address) == elle::Status::Error)
            escape("unable to bind the contents");

          // set the content as consistent.
          context.contents->state = nucleus::proton::StateConsistent;

          // update the object.
          if (context.object->Update(
                context.author,
                address,
                size,
                context.object->access(),
                context.object->owner_token()) == elle::Status::Error)
            escape("unable to update the object");

          // mark the block as needing to be stored.
          if (context.transcript.Push(address,
                                      context.contents) == elle::Status::Error)
            escape("unable to record the object for storing");

          //
          // finally, since the data has been re-encrypted, the key must be
          // distributed to the users having been granted the read
          // permission.
          //

          // open the access.
          if (Access::Open(context) == elle::Status::Error)
            escape("unable to open the access block");

          // upgrade the access entries with the new key.
          if (Access::Upgrade(context, key) == elle::Status::Error)
            escape("unable to upgrade the accesses");
        }

      return elle::Status::Ok;
    }

  }
}

#endif
