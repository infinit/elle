#ifndef ETOILE_AUTOMATON_CONTENTS_HXX
# define ETOILE_AUTOMATON_CONTENTS_HXX

# include <cryptography/SecretKey.hh>

// XXX[temporary: for cryptography]
using namespace infinit;

# include <nucleus/proton/Revision.hh>
# include <nucleus/proton/Address.hh>
# include <nucleus/proton/State.hh>
# include <nucleus/proton/Porcupine.hh>
# include <nucleus/proton/Limits.hh>
# include <nucleus/neutron/Permissions.hh>
# include <nucleus/neutron/Size.hh>
# include <nucleus/neutron/Data.hh>
# include <nucleus/neutron/Catalog.hh>
# include <nucleus/neutron/Reference.hh>

# include <etoile/automaton/Rights.hh>
# include <etoile/automaton/Author.hh>
# include <etoile/automaton/Access.hh>

# include <etoile/nest/Nest.hh>

# include <etoile/depot/Depot.hh>

# include <hole/Hole.hh>
# include <agent/Agent.hh>

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
      ELLE_TRACE_FUNCTION(context);

      // if the contents is already opened, return.
      if (context.porcupine != nullptr)
        return elle::Status::Ok;

      // Check if there exists a contents. if so, instanciate a porcupine.
      if (context.object->contents().empty() == false)
        {
          // determine the rights the current user has on this object.
          if (Rights::Determine(context) == elle::Status::Error)
            escape("unable to determine the user's rights");

          // if the user has the permission to read, decrypt the content.
          if ((context.rights.permissions &
               nucleus::neutron::permissions::read) ==
              nucleus::neutron::permissions::read)
            {
              // XXX
              nucleus::proton::Limits limits{
                nucleus::proton::limits::Porcupine{},
                nucleus::proton::limits::Node{1024, 0.5, 0.2},
                nucleus::proton::limits::Node{1024, 0.5, 0.2}};

              // Instanciate a nest.
              context.nest = new etoile::nest::Nest{limits};

              // Instanciate a porcupine.
              context.porcupine =
                new nucleus::proton::Porcupine<typename T::C>{
                  context.object->contents(),
                  *context.rights.key,
                  *context.nest};
            }
          else
            {
              // Otherwise, the user does not have the permission, hence keep
              // the porcupine null so that, if it acquires the permission,
              // the code above is executed, decrypting the content.
            }
        }
      else
        {
          // XXX
          nucleus::proton::Limits limits{
            nucleus::proton::limits::Porcupine{},
            nucleus::proton::limits::Node{1024, 0.5, 0.2},
            nucleus::proton::limits::Node{1024, 0.5, 0.2}};

          // Instanciate a nest.
          context.nest = new etoile::nest::Nest{limits};

          // otherwise create a new empty porcupine.
          context.porcupine =
            new nucleus::proton::Porcupine<typename T::C>{*context.nest};
        }

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
      ELLE_TRACE_FUNCTION(context);

      // If the object holds some content, mark the blocks as needing removal.
      if (context.object->contents().empty() == false)
        {
          // Optimisation: only proceed if the content strategy is block-based:
          switch (context.object->contents().strategy())
            {
            case nucleus::proton::Strategy::none:
              throw elle::Exception("unable to destroy an empty content");
            case nucleus::proton::Strategy::value:
              {
                // Nothing to do in this case since there is no block for
                // holding the content.
                //
                // The optimization makes us save some time since the content
                // is neither deserialized nor decrypted.

                break;
              }
            case nucleus::proton::Strategy::block:
            case nucleus::proton::Strategy::tree:
              {
                // open the contents.
                if (Contents::Open(context) == elle::Status::Error)
                  escape("unable to open the contents");

                ELLE_TRACE("record the content blocks for removal");
                ELLE_ASSERT(context.porcupine != nullptr);
                context.porcupine->destroy();

                ELLE_TRACE("mark the destroyed blocks as needed to be "
                           "removed from the storage layer");
                context.nest->record(context.transcript);

                break;
              }
            default:
              throw elle::Exception("unknown strategy '%s'",
                                    context.object->contents().strategy());
            }
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
      ELLE_TRACE_FUNCTION(context);

      //
      // first, check if the block has been modified i.e exists and is dirty.
      //
      {
        // if there is no loaded contents or accessible content, then there
        // is nothing to do.
        if (context.porcupine == nullptr)
          return elle::Status::Ok;

        // if the contents has not changed, do nothing.
        if (context.porcupine->state() == nucleus::proton::State::clean)
          return elle::Status::Ok;
      }

      ELLE_TRACE("the content seems to have been modified");

      // retrieve the contents's size.
      nucleus::neutron::Size size = context.porcupine->size();

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
          // revisions, removing the content block would render the previous
          // revision inconsistent.
          //
          // therefore, the object is updated with a null content address.
          //
          // however, should the history functionality not be supported
          // by the network, the access block is marked for deletion.
          //

          ELLE_TRACE("the Contents block is empty");

          // does the network support the history?
          // XXX: restore history handling
          // if (depot::hole().descriptor().meta().history() == false)
            {
              // destroy the contents block.
              if (Contents::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the contents block");
            }

          ELLE_TRACE("update the object with a null Contents address")
            {
              // update the object with the null contents address.
              if (context.object->Update(
                    *context.author,
                    nucleus::proton::Radix{},
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
          // objects benefit from the history i.e multiple revisions; unless
          // the history support is not activated for this network.
          //
          ELLE_TRACE("the Contents block is _not_ empty");

          // does the network support the history?
          // XXX: restore history handling
          // if (depot::hole().descriptor().meta().history() == false)
          /* XXX[porcupine: now the ancien blocks are not removed but replaced
                 and everything is handled by porcupine/nest]
            {
              // destroy the contents block.
              if (Contents::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the contents block");
            }
          */

          // XXX[should provide a len but with a static const value]
          cryptography::SecretKey key{cryptography::SecretKey::generate(256)};

          // update the object.
          if (context.object->Update(
                *context.author,
                context.porcupine->seal(key),
                size,
                context.object->access(),
                context.object->owner_token()) == elle::Status::Error)
            escape("unable to update the object");

          // mark the new/modified blocks as needing to be stored.
          context.nest->record(context.transcript);

          // XXX
          printf("TRANSCRIPT\n");
          context.transcript.Dump();

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
