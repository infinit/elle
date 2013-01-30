#include <etoile/automaton/Attributes.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/gear/Object.hh>
#include <etoile/nest/Nest.hh>
#include <etoile/depot/Depot.hh>

#include <nucleus/proton/Porcupine.hh>
#include <nucleus/proton/Door.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Trait.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Attributes.hh>

#include <agent/Agent.hh>

#include <elle/log.hh>

#include <elle/idiom/Close.hh>
# include <limits>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.etoile.automaton.Attributes");

// XXX
#define ATTRIBUTES_SECRET_KEY_LENGTH 13
#define ATTRIBUTES_SECRET_KEY "no-secret-key"

namespace etoile
{
  namespace automaton
  {

//
// ---------- methods ---------------------------------------------------------
//

    void
    Attributes::open(gear::Object& context)
    {
      ELLE_TRACE_FUNCTION(context);

      // XXX
      static cryptography::SecretKey secret_key(cryptography::cipher::Algorithm::aes256,
                                                ATTRIBUTES_SECRET_KEY);

      if (context.attributes_porcupine != nullptr)
        return;

      // Check if there exists attributes. if so, instanciate a porcupine.
      if (context.object->attributes().empty() == false)
        {
          // Instanciate a nest.
          context.attributes_nest =
            new etoile::nest::Nest(ATTRIBUTES_SECRET_KEY_LENGTH,
                                   context.attributes_limits,
                                   depot::hole().storage().network(),
                                   agent::Agent::Subject.user());

          // Instanciate a porcupine.
          context.attributes_porcupine =
            new nucleus::proton::Porcupine<nucleus::neutron::Attributes>{
              context.object->attributes(),
              secret_key,
              *context.attributes_nest};
        }
      else
        {
          // Instanciate a nest.
          context.attributes_nest =
            new etoile::nest::Nest(ATTRIBUTES_SECRET_KEY_LENGTH,
                                   context.attributes_limits,
                                   depot::hole().storage().network(),
                                   agent::Agent::Subject.user());

          // otherwise create a new empty porcupine.
          context.attributes_porcupine =
            new nucleus::proton::Porcupine<nucleus::neutron::Attributes>{
              *context.attributes_nest};
        }

      ELLE_ASSERT(context.attributes_porcupine != nullptr);
    }

    ///
    /// this method sets an attribute.
    ///
    elle::Status        Attributes::Set(
                          gear::Object&                         context,
                          const elle::String&                   name,
                          const elle::String&                   value)
    {
      ELLE_TRACE_FUNCTION(context, name, value);

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // verify that the user can modify the attributes.
      if (context.rights.role != nucleus::neutron::Object::RoleOwner)
        escape("the user does not seem to have the permission to update "
               "this object's attributes");

      Attributes::open(context);

      // Retrieve a door on the attributes.
      nucleus::proton::Door<nucleus::neutron::Attributes> door{
        context.attributes_porcupine->lookup(name)};

      door.open();

      // does the attribute already exist.
      if (door().exist(name) == true)
        {
          // update the trait.
          door().update(name, value);
        }
      else
        {
          // allocate a new trait.
          nucleus::neutron::Trait* trait =
            new nucleus::neutron::Trait(name, value);

          // add the trait to the attributes.
          door().insert(trait);
        }

      door.close();

      // Update the porcupine.
      context.attributes_porcupine->update(name);

      // administrate the object.
      if (context.object->Administrate(
            context.object->attributes(),
            context.object->owner_permissions()) == elle::Status::Error)
        escape("unable to administrate the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    ///
    /// this method looks for the given name in the attributes and
    /// return the associated value.
    ///
    elle::Status        Attributes::Get(
                          gear::Object&                         context,
                          const elle::String&                   name,
                          nucleus::neutron::Trait const*& trait)
    {
      ELLE_TRACE_FUNCTION(context, name);

      Attributes::open(context);

      // Retrieve a door on the attributes.
      nucleus::proton::Door<nucleus::neutron::Attributes> door{
        context.attributes_porcupine->lookup(name)};

      door.open();

      // XXX[does the trait exist: if not, return a null pointer]
      if (door().exist(name) == false)
        {
          trait = nullptr;
        }
      else
        {
          // Look up the entry.
          // XXX[we take the address of the reference: wrong]
          trait = &(door().locate(name));
        }

      door.close();

      return elle::Status::Ok;
    }

    ///
    /// this method returns all the attributes.
    ///
    elle::Status        Attributes::Fetch(
                          gear::Object&                         context,
                          nucleus::neutron::Range<
                            nucleus::neutron::Trait>& range)
    {
      ELLE_TRACE_FUNCTION(context);

      Attributes::open(context);

      // Seek the attributes responsible for the given index.
      nucleus::proton::Capacity _index = 0;
      auto _size = context.attributes_porcupine->size();

      while (_size > 0)
        {
          auto pair = context.attributes_porcupine->seek(_index);
          auto& door = pair.first;
          auto& base = pair.second;

          door.open();

          auto start = _index - base;
          auto length = _size > (door().size() - start) ?
            (door().size() - start) : _size;

          ELLE_ASSERT(length != 0);

          // Retrieve the traits falling in the requested
          // range [index, index + size[.
          nucleus::neutron::Range<nucleus::neutron::Trait> subrange{
            door().consult(start, length)};

          door.close();

          // Inject the retrieved traits into the main range.
          range.add(subrange);

          // Update the variables _index and _size.
          _index += length;
          _size -= length;
        }

      return elle::Status::Ok;
    }

    ///
    /// this method removes an attribute.
    ///
    elle::Status        Attributes::Omit(
                          gear::Object&                         context,
                          const elle::String&                   name)
    {
      ELLE_TRACE_FUNCTION(context, name);

      // determine the rights over the object.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // verify that the user can modify the attributes.
      if (context.rights.role != nucleus::neutron::Object::RoleOwner)
        escape("the user does not seem to have the permission to update "
               "this object's attributes");

      Attributes::open(context);

      // Retrieve a door on the attributes.
      nucleus::proton::Door<nucleus::neutron::Attributes> door{
        context.attributes_porcupine->lookup(name)};

      door.open();

      // remove the trait.
      door().erase(name);

      door.close();

      // Update the porcupine.
      context.attributes_porcupine->update(name);

      // administrate the object.
      if (context.object->Administrate(
            context.object->attributes(),
            context.object->owner_permissions()) == elle::Status::Error)
        escape("unable to administrate the object");

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    void
    Attributes::destroy(gear::Object& context)
    {
      ELLE_TRACE_FUNCTION(context);

      // If the object holds some attributes, mark the blocks as
      // needing removal.
      if (context.object->attributes().empty() == false)
        {
          // Optimisation: only proceed if the content strategy is block-based:
          switch (context.object->attributes().strategy())
            {
            case nucleus::proton::Strategy::none:
              throw elle::Exception("unable to destroy an empty content");
            case nucleus::proton::Strategy::value:
              {
                // Nothing to do in this case since there is no block for
                // holding the attributes.
                //
                // The optimization makes us save some time since the content
                // is not deserialized.

                break;
              }
            case nucleus::proton::Strategy::block:
            case nucleus::proton::Strategy::tree:
              {
                Attributes::open(context);

                ELLE_TRACE("record the attributes blocks for removal");
                ELLE_ASSERT(context.attributes_porcupine != nullptr);
                context.attributes_porcupine->destroy();

                ELLE_TRACE("mark the destroyed blocks as needed to be "
                           "removed from the storage layer");
                context.transcript().merge(
                  context.attributes_nest->transcribe());

                break;
              }
            default:
              throw elle::Exception("unknown strategy '%s'",
                                    context.object->attributes().strategy());
            }
        }
    }

    void
    Attributes::close(gear::Object& context)
    {
      ELLE_TRACE_FUNCTION(context);

      //
      // first, check if the block has been modified i.e exists and is dirty.
      //
      {
        // if there is no loaded content, then there is nothing to do.
        if (context.attributes_porcupine == nullptr)
          return;

        // if the attributes have not changed, do nothing.
        if (context.attributes_porcupine->state() ==
            nucleus::proton::State::clean)
          return;
      }

      ELLE_TRACE("the attributes seem to have been modified");

      // retrieve the attributes's size.
      nucleus::neutron::Size size = context.attributes_porcupine->size();

      //
      // at this point, the attributes is known to have been modified.
      //

      // modify the object according to the content.
      if (size == 0)
        {
          //
          // if the attributes became empty after removals, the
          // object should no longer point to any attributes block while
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

          ELLE_TRACE("the attributes are empty");

          // does the network support the history?
          // XXX: restore history handling
          // if (depot::hole().descriptor().meta().history() == false)
            {
              // destroy the attributes block.
              Attributes::destroy(context);
            }

          ELLE_TRACE("administrate the object with a null Attributes");

          if (context.object->Administrate(
                    nucleus::proton::Radix{},
                    context.object->owner_permissions()) == elle::Status::Error)
            escape("unable to update the object");
        }
      else
        {
          //
          // otherwise, the new attributes address needs to be computed and
          // the object updated accordingly.
          //
          // note that the previous content block is not removed since
          // objects benefit from the history i.e multiple revisions; unless
          // the history support is not activated for this network.
          //
          ELLE_TRACE("the attributes are _not_ empty");

          // does the network support the history?
          // XXX: restore history handling
          // if (depot::hole().descriptor().meta().history() == false)
          /* XXX[porcupine: now the ancient blocks are not removed but replaced
                 and everything is handled by porcupine/nest]
            {
              // destroy the attributes block.
              if (Attributes::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the attributes block");
            }
          */

          // XXX
          static cryptography::SecretKey secret_key(cryptography::cipher::Algorithm::aes256,
                                                    ATTRIBUTES_SECRET_KEY);

          // administrate the object.
          if (context.object->Administrate(
                context.attributes_porcupine->seal(secret_key),
                context.object->owner_permissions()) == elle::Status::Error)
            escape("unable to update the object");

          // XXX[too slow without a nest optimization: to activate later]
          ELLE_STATEMENT(context.attributes_porcupine->check(nucleus::proton::flags::all));

          // mark the new/modified blocks as needing to be stored.
          context.transcript().merge(context.attributes_nest->transcribe());
        }
    }
  }
}
