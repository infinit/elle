#include <nucleus/neutron/Data.hh>

#include <etoile/automaton/File.hh>
#include <etoile/automaton/Object.hh>
#include <etoile/automaton/Contents.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/gear/File.hh>

#include <agent/Agent.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.etoile.automaton.File");

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
      ELLE_TRACE_FUNCTION(context);

      ELLE_ASSERT(context.object == nullptr);

      context.object.reset(
        new nucleus::neutron::Object(nucleus::proton::Network(Infinit::Network),
                                     agent::Agent::Identity.pair().K(),
                                     nucleus::neutron::Genre::file));

      nucleus::proton::Address address(context.object->bind());

      // create the context's location with an initial revision number.
      context.location =
        nucleus::proton::Location(address, context.object->revision());

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
      ELLE_TRACE_FUNCTION(context);

      // return if the context has already been loaded.
      if (context.state != gear::Context::StateUnknown)
        return elle::Status::Ok;

      // load the object.
      if (Object::Load(context) == elle::Status::Error)
        escape("unable to fetch the object");

      // check that the object is a file.
      if (context.object->genre() != nucleus::neutron::Genre::file)
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
                          const nucleus::neutron::Offset& offset,
                          const elle::standalone::Region& region)
    {
      ELLE_TRACE_FUNCTION(context, offset, region);

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the write the data.
      if ((context.rights.permissions & nucleus::neutron::permissions::write) !=
          nucleus::neutron::permissions::write)
        escape("the user does not seem to have the permission to write "
               "this file");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.porcupine == nullptr)
        escape("the user does not seem to be able to operate on this "
               "file");

      // Retrieve a door on the data.
      nucleus::proton::Door<nucleus::neutron::Data> door{
        context.porcupine->lookup(offset)};

      door.open();

      // Write the content in the appropriate data node.
      door().write(offset, elle::WeakBuffer{region.contents, region.size});

      door.close();

      // Update the porcupine.
      context.porcupine->update(offset);

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

      // XXX
      context.porcupine->dump();

      return elle::Status::Ok;
    }

    ///
    /// this method returns a specific region of the file.
    ///
    elle::Status        File::Read(
                          gear::File&                           context,
                          const nucleus::neutron::Offset& offset,
                          const nucleus::neutron::Size& size,
                          elle::standalone::Region&                         region)
    {
      ELLE_TRACE_FUNCTION(context, offset, size);

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the read the data.
      if ((context.rights.permissions & nucleus::neutron::permissions::read) !=
          nucleus::neutron::permissions::read)
        escape("the user does not seem to have the permission to read "
               "this file");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.porcupine == nullptr)
        escape("the user does not seem to be able to operate on this "
               "file");

      // Check that there is enough data to be read.
      if (offset > context.porcupine->size())
        return elle::Status::Ok;

      // Retrieve a door on the data.
      nucleus::proton::Door<nucleus::neutron::Data> door{
        context.porcupine->lookup(offset)};

      door.open();

      // Read the content in the appropriate data node.
      elle::Buffer buffer = door().read(offset, size);

      door.close();

      // XXX[not optimized: migrate to elle::Buffer so as to avoid
      //     such stupid copies]
      region.Prepare(buffer.size());
      region.Write(0, buffer.contents(), buffer.size());

      return elle::Status::Ok;
    }

    ///
    /// this method adjusts the size of the file's contents.
    ///
    elle::Status        File::Adjust(
                          gear::File&                           context,
                          const nucleus::neutron::Size& size)
    {
      ELLE_TRACE_FUNCTION(context, size);

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user has the right the adjust the file.
      if ((context.rights.permissions & nucleus::neutron::permissions::write) !=
          nucleus::neutron::permissions::write)
        escape("the user does not seem to have the permission to adjust "
               "this file");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.porcupine == nullptr)
        escape("the user does not seem to be able to operate on this "
               "file");

      // Ignore the call should the requested size be identical to the existing
      // one.
      if (size == context.porcupine->size())
        return elle::Status::Ok;

      // Trim the porcupine manually depending on the content strategy. In other
      // words, triming a tree is a bit specific as blocks must be removed
      // manually.
      switch (context.porcupine->strategy())
        {
        case nucleus::proton::Strategy::none:
          throw elle::Exception("invalid 'none' strategy");
        case nucleus::proton::Strategy::value:
        case nucleus::proton::Strategy::block:
          {
            nucleus::proton::Door<nucleus::neutron::Data> door{
              context.porcupine->lookup(size)};

            door.open();

            // Directly adjsut the data.
            door().adjust(size - door().offset());

            // Update the porcupine.
            context.porcupine->update(size);

            door.close();

            break;
          }
        case nucleus::proton::Strategy::tree:
          {
            // Retrieve a door on the data responsible for the
            // offset _size_.
            nucleus::proton::Door<nucleus::neutron::Data> base{
              context.porcupine->lookup(size)};

            // Act depending on the fact that the file is shrunk or
            // extended.
            ELLE_ASSERT(size != context.porcupine->size());
            /* XXX
            if (size < context.porcupine->size())
              {
                // The file is shrunk.
                //
                // All the blocks following one responsible for _size_
                // are removed.

                base.open();

                // Check whether there are additional blocks after the current
                // one i.e the current block is not responsible for the end of
                // the file.
                if (context.porcupine->size() >
                    (base().offset() + base().size()))
                  {
                    elle::Boolean
                    while (true)
                      {
                        // Retrieve the door on the node following _base_.
                        nucleus::proton::Door<nucleus::neutron::Data> door{
                          context.porcupine->lookup(
                            base().offset() + base().size())};

                        door.open();

                        door.close();
                        };
                        }

// XXX no need to update because remove does it

                base.close();
              }
            else
              {
                // The file is extended

                XXX error si on ajoute trop d'un coup l'optim va chier non?
              }
            */

            // Update the porcupine because the tree needs to be optimised
            // following the adjustment of the data node.
            context.porcupine->update(size);

            break;
          }
        default:
          throw elle::Exception("unknown strategy '%s'",
                                context.porcupine->strategy());
        }

        // XXX les doors doivent etre ouvertes/fermees correctement.

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
    elle::Status        File::Discard(
                          gear::File&                           context)
    {
      ELLE_TRACE_FUNCTION(context);

      // discard the object-related information.
      if (Object::Discard(context) == elle::Status::Error)
        escape("unable to discard the object");

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
      ELLE_TRACE_FUNCTION(context);

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context.rights.role != nucleus::neutron::Object::RoleOwner)
        escape("the user does not seem to have the permission to destroy "
               "this file");

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
