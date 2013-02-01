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
        throw elle::Exception("unable to fetch the object");

      // check that the object is a file.
      if (context.object->genre() != nucleus::neutron::Genre::file)
        throw elle::Exception("this object does not seem to be a file");

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

      // Ignore such zero-sized requests
      if (region.size == 0)
        return elle::Status::Ok;

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        throw elle::Exception("unable to determine the rights");

      // check if the current user has the right the write the data.
      if ((context.rights.permissions & nucleus::neutron::permissions::write) !=
          nucleus::neutron::permissions::write)
        throw elle::Exception("the user does not seem to have the permission to write "
               "this file");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        throw elle::Exception("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents_porcupine == nullptr)
        throw elle::Exception("the user does not seem to be able to operate on this "
               "file");

      // Initialize the offset which will move forward until it reaches the
      // size.
      //
      // Note that the size represents the number of bytes to be overwritten,
      // the expanding process taking place afterwards.
      nucleus::neutron::Offset absolute_offset = offset;
      nucleus::neutron::Size absolute_size =
        offset < context.contents_porcupine->size() ?
          ((offset + region.size) < context.contents_porcupine->size() ?
           region.size : (context.contents_porcupine->size() - offset)) :
          0;

      // Write the content [offset, offset + size[ which may span over
      // several data blocks.
      while (absolute_offset < (offset + absolute_size))
        {
          // Retrieve a door on the data.
          nucleus::proton::Door<nucleus::neutron::Data> data{
            context.contents_porcupine->lookup(absolute_offset)};

          data.open();

          // Compute the offset relative to the door.
          nucleus::neutron::Offset relative_offset =
            absolute_offset - data().offset();

          // Compute the size of the content to write depending
          // on the size of the current data block.
          nucleus::neutron::Size relative_size =
            (data().size() - relative_offset) <
              (absolute_size - (absolute_offset - offset)) ?
            (data().size() - relative_offset) :
              (absolute_size - (absolute_offset - offset));

          ELLE_TRACE("write %s bytes of content at the absolute offset %s",
                     relative_size, absolute_offset);

          // Write the content in the appropriate data node.
          ELLE_ASSERT(relative_size != 0);

          data().write(relative_offset,
                       elle::WeakBuffer{
                         region.contents + (absolute_offset - offset),
                         relative_size});

          data.close();

          context.contents_porcupine->update(absolute_offset);

          // Update the current offset so as to move on to the
          // next block until the requested content has been written.
          absolute_offset = absolute_offset + relative_size;
          ELLE_TRACE("proceed to the next block at the absolute offset %s with "
                     "%s remaining to be read",
                     absolute_offset,
                     absolute_size - (absolute_offset - offset));
          ELLE_ASSERT(absolute_offset <= (offset + absolute_size));
        }

      // Then, proceed to extending the file, should it be necessary.
      //
      // If the end of the current content has been reached (i.e the
      // last block) and data remains to be written, extend the last
      // block with the remaining content and finally stop.
      ELLE_ASSERT(absolute_offset <= context.contents_porcupine->size());

      ELLE_TRACE("the writing reached offset %s of the total size %s",
                 absolute_offset, context.contents_porcupine->size());

      if (absolute_offset == context.contents_porcupine->size())
        {
          // Compute the expanding size.
          nucleus::neutron::Size expanding_size = region.size;

          ELLE_TRACE("the end offset %s of the current content has been "
                     "reached with %s bytes remained to be written",
                     absolute_offset,
                     expanding_size - (absolute_offset - offset));

          // Check if content remains to be written.
          if ((offset + expanding_size) > context.contents_porcupine->size())
            {
              ELLE_TRACE("expand the last data block with %s bytes of data",
                         expanding_size - (absolute_offset - offset));

              // Retrieve the very last data block.
              nucleus::proton::Door<nucleus::neutron::Data> end{
                context.contents_porcupine->tail()};

              // Write the remaining content at the end of the last
              // data block.
              end().write(end().size(),
                          elle::WeakBuffer{
                           region.contents + (absolute_offset - offset),
                           expanding_size - (absolute_offset - offset)});

              context.contents_porcupine->update(absolute_offset);

              end.close();
            }
        }

      ELLE_ASSERT(context.contents_porcupine->size() >= region.size);

      // update the object.
      if (context.object->Update(
            context.object->author(),
            context.object->contents(),
            context.contents_porcupine->size(),
            context.object->access(),
            context.object->owner_token()) == elle::Status::Error)
        throw elle::Exception("unable to update the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

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

      // Ignore such zero-sized requests.
      if (size == 0)
        return elle::Status::Ok;

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        throw elle::Exception("unable to determine the rights");

      // check if the current user has the right the read the data.
      if ((context.rights.permissions & nucleus::neutron::permissions::read) !=
          nucleus::neutron::permissions::read)
        throw elle::Exception("the user does not seem to have the permission to read "
               "this file");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        throw elle::Exception("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents_porcupine == nullptr)
        throw elle::Exception("the user does not seem to be able to operate on this "
               "file");

      // Check that there is enough data to be read.
      if (offset > context.contents_porcupine->size())
        return elle::Status::Ok;

      // Initialize the offset which will move forward until it reaches
      // the size.
      nucleus::neutron::Offset absolute_offset = offset;
      nucleus::neutron::Size absolute_size =
        (offset + size) < context.contents_porcupine->size() ?
        size : (context.contents_porcupine->size() - offset);

      // Return if there is no more data to read.
      if (absolute_size == 0)
        return elle::Status::Ok;

      ELLE_TRACE("about to read %s bytes of data at offset %s from "
                 "a porcupine of size %s",
                 absolute_size, absolute_offset,
                 context.contents_porcupine->size());

      // Read the content [offset, offset + size[ which may span over
      // several data blocks.
      elle::Buffer buffer;

      while (absolute_offset < (offset + absolute_size))
        {
          // Retrieve a door on the data.
          nucleus::proton::Door<nucleus::neutron::Data> data{
            context.contents_porcupine->lookup(absolute_offset)};

          data.open();

          // Compute the offset relative to the door.
          nucleus::neutron::Offset relative_offset =
            absolute_offset - data().offset();

          // Compute the size of the content to read depending
          // on what is requested and what is available.
          nucleus::neutron::Size relative_size =
            (data().size() - relative_offset) <
              (absolute_size - (absolute_offset - offset)) ?
            (data().size() - relative_offset) :
              (absolute_size - (absolute_offset - offset));

          ELLE_ASSERT(relative_size != 0);

          ELLE_TRACE("reading %s bytes at the absolute offset %s",
                     relative_size, absolute_offset);

          // Read the content in the appropriate data node.
          data().read(relative_offset, relative_size, buffer);

          data.close();

          // Update the current offset so as to move on to the
          // next block until the requested content has been read.
          absolute_offset = absolute_offset + relative_size;
          ELLE_TRACE("proceed to the next block at the absolute offset %s with "
                     "%s remaining to be read",
                     absolute_offset,
                     absolute_size - (absolute_offset - offset));
          ELLE_ASSERT(absolute_offset <= (offset + absolute_size));
        }

      ELLE_ASSERT(buffer.size() != 0);
      ELLE_ASSERT(buffer.size() <= size);

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
        throw elle::Exception("unable to determine the rights");

      // check if the current user has the right the adjust the file.
      if ((context.rights.permissions & nucleus::neutron::permissions::write) !=
          nucleus::neutron::permissions::write)
        throw elle::Exception("the user does not seem to have the permission to adjust "
               "this file");

      // open the contents.
      if (Contents::Open(context) == elle::Status::Error)
        throw elle::Exception("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents_porcupine == nullptr)
        throw elle::Exception("the user does not seem to be able to operate on this "
               "file");

      // Ignore the call should the requested size be identical to the existing
      // one.
      if (size == context.contents_porcupine->size())
        return elle::Status::Ok;

      // Trim the porcupine manually depending on the content strategy. In other
      // words, triming a tree is a bit specific as blocks must be removed
      // manually.
      switch (context.contents_porcupine->strategy())
        {
        case nucleus::proton::Strategy::none:
          throw elle::Exception("invalid 'none' strategy");
        case nucleus::proton::Strategy::value:
        case nucleus::proton::Strategy::block:
          {
            // Retrieve a door on the only block.
            nucleus::proton::Door<nucleus::neutron::Data> door{
              context.contents_porcupine->lookup(size)};

            // Directly adjust the size, relatively i.e size - door.offset.
            door.open();

            ELLE_ASSERT(door().offset() == 0);
            door().adjust(size - door().offset());

            door.close();

            // Update the porcupine.
            context.contents_porcupine->update(size);

            break;
          }
        case nucleus::proton::Strategy::tree:
          {
            // Retrieve a door on the data responsible for the
            // offset _size_.
            nucleus::proton::Door<nucleus::neutron::Data> base{
              context.contents_porcupine->lookup(size)};

            // Act depending on the fact that the file is shrunk or
            // extended.
            ELLE_ASSERT(size != context.contents_porcupine->size());

            if (size < context.contents_porcupine->size())
              {
                // The file is shrunk.
                //
                // All the blocks following the one (i.e _base_) responsible
                // for _size_ are removed.

                while (true)
                  {
                    // Retrieve the door on the last data node.
                    nucleus::proton::Door<nucleus::neutron::Data> end{
                      context.contents_porcupine->tail()};

                    // If this node is the same as _base_, this means
                    // _base_ is the last node; hence stop.
                    //
                    // Otherwise remove the node from the tree.
                    if (end == base)
                      break;
                    else
                      {
                        // Retrieve the key for which the last node is
                        // responsible.
                        end.open();

                        nucleus::neutron::Offset _key = end().offset();

                        end.close();

                        // Remove the data block.
                        context.contents_porcupine->tree().remove(_key);

                        // Although the remove() operation updates the
                        // tree, the porcupine needs updating as well so
                        // as to maintain consistency regarding the state.
                        //
                        // However, since the _base_ node will be adjusted,
                        // the porcupine will be updated. Therefore, delay
                        // the update for optimization purposes.
                      }
                  }

                // Finally, the _base_ node needs to be adjusted to the final
                // size, which is relative to the _base_: size - base.offset
                base.open();

                base().adjust(size - base().offset());

                base.close();
              }
            else
              {
                // The file is extended.

                // Retrieve the door on the last data node.
                nucleus::proton::Door<nucleus::neutron::Data> end{
                  context.contents_porcupine->tail()};

                // Adjust the size of the last node, relatively i.e
                // size - end.offset.
                end.open();

                end().adjust(size - end().offset());

                end.close();
              }

            // Update the porcupine because the tree needs to be optimised
            // following the adjustment of the data node.
            context.contents_porcupine->update(size);

            break;
          }
        default:
          throw elle::Exception("unknown strategy '%s'",
                                context.contents_porcupine->strategy());
        }

      // update the object.
      if (context.object->Update(
            context.object->author(),
            context.object->contents(),
            context.contents_porcupine->size(),
            context.object->access(),
            context.object->owner_token()) == elle::Status::Error)
        throw elle::Exception("unable to update the object");

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
        throw elle::Exception("unable to discard the object");

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
        throw elle::Exception("unable to determine the rights");

      // check if the current user is the object owner.
      if (context.rights.role != nucleus::neutron::Object::RoleOwner)
        throw elle::Exception("the user does not seem to have the permission to destroy "
               "this file");

      // destroy the contents.
      if (Contents::Destroy(context) == elle::Status::Error)
        throw elle::Exception("unable to destroy the contents");

      // destroy the object-related information.
      if (Object::Destroy(context) == elle::Status::Error)
        throw elle::Exception("unable to destroy the object");

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
        throw elle::Exception("unable to close the contents");

      // store the object-related information.
      if (Object::Store(context) == elle::Status::Error)
        throw elle::Exception("unable to store the object");

      // set the context's state.
      context.state = gear::Context::StateStored;

      return elle::Status::Ok;
    }

  }
}
