#include <boost/format.hpp>

#include <lune/Descriptor.hh>
#include <lune/Set.hh>
#include <lune/Passport.hh>

#include <nucleus/proton/Address.hh>

#include <hole/Hole.hh>
#include <Infinit.hh>

namespace hole
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "hole";

  ///
  /// this variable contains the network descriptor.
  ///
  lune::Descriptor              Hole::Descriptor;

  ///
  /// this variable contains the network set of initial nodes.
  ///
  lune::Set                     Hole::Set;

  ///
  /// this variable contains the device passport.
  ///
  lune::Passport                Hole::Passport;

  ///
  /// this variable holds the hole implementation.
  ///
  Holeable*                     Hole::Implementation = NULL;

  ///
  /// XXX
  ///
  Hole::State                   Hole::state = Hole::StateOffline;

  ///
  /// XXX[to replace by a new signal]
  ///
  elle::concurrency::Signal<
    elle::radix::Parameters<>
    >                           Hole::ready;

//
// ---------- static methods --------------------------------------------------
//

  void
  Hole::Initialize()
  {
    nucleus::Network    network;

    // disable the meta logging.
    if (elle::Meta::Disable() == elle::Status::Error)
      throw std::runtime_error("unable to disable the meta logging");

    //
    // retrieve the descriptor.
    //
    {
      // does the network exist.
      if (Hole::Descriptor.Exist(Infinit::Network) == elle::Status::False)
        throw std::runtime_error("this network does not seem to exist");

      // load the descriptor.
      if (Hole::Descriptor.Load(Infinit::Network) == elle::Status::Error)
        throw std::runtime_error("unable to load the descriptor");

      // validate the descriptor.
      if (Hole::Descriptor.Validate(Infinit::Authority) == elle::Status::Error)
        throw std::runtime_error("unable to validate the descriptor");
    }

    //
    // retrieve the set, if present.
    //
    if (Hole::Set.Exist(Infinit::Network) == elle::Status::True)
      {
        // load the set.
        if (Hole::Set.Load(Infinit::Network) == elle::Status::Error)
          throw std::runtime_error("unable to load the set");
      }

    //
    // retrieve the passport.
    //
    {
      // does the network exist.
      if (Hole::Passport.Exist() == elle::Status::False)
        throw std::runtime_error("the device passport does not seem to exist");

      // load the passport.
      if (Hole::Passport.Load() == elle::Status::Error)
        throw std::runtime_error("unable to load the passport");

      // validate the passport.
      if (Hole::Passport.Validate(Infinit::Authority) == elle::Status::Error)
        throw std::runtime_error("unable to validate the passport");
    }

    // enable the meta logging.
    if (elle::Meta::Enable() == elle::Status::Error)
      throw std::runtime_error("unable to enable the meta logging");

    // create the network instance.
    if (network.Create(Infinit::Network) == elle::Status::Error)
      throw std::runtime_error("unable to create the network instance");

    // create the holeable depending on the model.
    switch (Hole::Descriptor.model.type)
      {
      case Model::TypeLocal:
        {
          // allocate the instance.
          Hole::Implementation =
            new implementations::local::Implementation(network);

          break;
        }
      case Model::TypeRemote:
        {
          // allocate the instance.
          Hole::Implementation =
            new implementations::remote::Implementation(network);

          break;
        }
      case Model::TypeSlug:
        {
          // allocate the instance.
          Hole::Implementation =
            new implementations::slug::Implementation(network);

          break;
        }
      case Model::TypeCirkle:
        {
          /* XXX
          // allocate the instance.
          Hole::Implementation =
            new implementations::cirkle::Implementation(network);
          */

          break;
        }
      default:
        {
          static boost::format fmt("unknown or not-yet-supported model '%u'");
          throw std::runtime_error(str(fmt % Hole::Descriptor.model.type));
        }
      }

    // join the network
    Hole::Implementation->Join();
  }

  ///
  /// this method cleans the hole.
  ///
  /// the components are recycled just to make sure the memory is
  /// released before the Meta allocator terminates.
  ///
  elle::Status          Hole::Clean()
  {
    // leave the network
    if (Hole::Implementation->Leave() == elle::Status::Error)
      escape("unable to leave the network");

    // delete the implementation.
    delete Hole::Implementation;

    return elle::Status::Ok;
  }

  ///
  /// this method can be called to signal the other components that the
  /// network layer is ready to receive requests.
  ///
  elle::Status          Hole::Ready()
  {
    if (Hole::state == Hole::StateOnline)
      return elle::Status::Ok;

    if (Hole::ready.Emit() == elle::Status::Error)
      escape("unable to emit the signal");

    Hole::state = Hole::StateOnline;

    return elle::Status::Ok;
  }

  ///
  /// this method returns the address of the root block i.e the origin.
  ///
  elle::Status          Hole::Origin(nucleus::Address&          address)
  {
    // return the address.
    address = Hole::Descriptor.root;

    return elle::Status::Ok;
  }

  ///
  /// this method stores the given block.
  ///
  elle::Status          Hole::Push(const nucleus::Address&      address,
                                   const nucleus::Block&        block)
  {
    // XXX check the block's footprint which should not exceed Extent

    // forward the request depending on the nature of the block which
    // the address indicates.
    switch (address.family)
      {
      case nucleus::FamilyContentHashBlock:
        {
          const nucleus::ImmutableBlock*        ib;

          // cast to an immutable block.
          ib = static_cast<const nucleus::ImmutableBlock*>(&block);

          // store the immutable block.
          if (Hole::Implementation->Put(address, *ib) == elle::Status::Error)
            escape("unable to put the block");

          break;
        }
      case nucleus::FamilyPublicKeyBlock:
      case nucleus::FamilyOwnerKeyBlock:
      case nucleus::FamilyImprintBlock:
        {
          const nucleus::MutableBlock*          mb;

          // cast to a mutable block.
          mb = static_cast<const nucleus::MutableBlock*>(&block);

          // store the mutable block.
          if (Hole::Implementation->Put(address, *mb) == elle::Status::Error)
            escape("unable to put the block");

          break;
        }
      default:
        {
          escape("unknown block family '%u'",
                 address.family);
        }
      }

    return elle::Status::Ok;
  }

  ///
  /// this method returns the block associated with the given address.
  ///
  elle::Status          Hole::Pull(const nucleus::Address&      address,
                                   const nucleus::Version&      version,
                                   nucleus::Block&              block)
  {
    // forward the request depending on the nature of the block which
    // the addres indicates.
    switch (address.family)
      {
      case nucleus::FamilyContentHashBlock:
        {
          nucleus::ImmutableBlock*      ib;

          // cast to an immutable block.
          ib = static_cast<nucleus::ImmutableBlock*>(&block);

          // retrieve the immutable block.
          if (Hole::Implementation->Get(address, *ib) == elle::Status::Error)
            escape("unable to get the block");

          break;
        }
      case nucleus::FamilyPublicKeyBlock:
      case nucleus::FamilyOwnerKeyBlock:
      case nucleus::FamilyImprintBlock:
        {
          nucleus::MutableBlock*        mb;

          // cast to a mutable block.
          mb = static_cast<nucleus::MutableBlock*>(&block);

          // retrieve the mutable block.
          if (Hole::Implementation->Get(address, version,
                                        *mb) == elle::Status::Error)
            escape("unable to get the block");

          break;
        }
      default:
        {
          escape("unknown block family '%u'",
                 address.family);
        }
      }

    return elle::Status::Ok;
  }

  ///
  /// this method removes the block associated with the given address.
  ///
  elle::Status          Hole::Wipe(const nucleus::Address&      address)
  {
    // forward the kill request to the implementation.
    if (Hole::Implementation->Kill(address) == elle::Status::Error)
      escape("unable to erase the block");

    return elle::Status::Ok;
  }

}
