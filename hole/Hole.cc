#include <elle/log.hh>

#include <hole/Hole.hh>
#include <hole/Holeable.hh>
#include <hole/implementations/local/Implementation.hh>
#include <hole/implementations/remote/Implementation.hh>
#include <hole/implementations/slug/Implementation.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>

#include <Infinit.hh>

#include <elle/idiom/Close.hh>
# include <boost/format.hpp>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.hole.Hole");

namespace hole
{

//
// ---------- definitions -----------------------------------------------------
//

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
  Holeable*                     Hole::Implementation = nullptr;

  ///
  /// XXX
  ///
  Hole::State                   Hole::state = Hole::StateOffline;

  boost::signal<void ()> Hole::_ready;

//
// ---------- static methods --------------------------------------------------
//

  void
  Hole::Initialize()
  {
    nucleus::proton::Network network;

    // disable the meta logging.
    if (elle::radix::Meta::Disable() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to disable the meta logging");

    //
    // retrieve the descriptor.
    //
    {
      // does the network exist.
      if (lune::Descriptor::exists(Infinit::Network) == false)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "this network does not seem to exist");

      // load the descriptor.
      Hole::Descriptor.load(Infinit::Network);

      // validate the descriptor.
      if (Hole::Descriptor.Validate(Infinit::Authority) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "unable to validate the descriptor");
    }

    //
    // retrieve the set, if present.
    //
    if (lune::Set::exists(Infinit::Network) == true)
      Hole::Set.load(Infinit::Network);

    //
    // retrieve the passport.
    //
    {
      // does the network exist.
      if (lune::Passport::exists() == false)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "the device passport does not seem to exist");

      // load the passport.
      Hole::Passport.load();

      // validate the passport.
      if (Hole::Passport.Validate(Infinit::Authority) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "unable to validate the passport");
    }

    // enable the meta logging.
    if (elle::radix::Meta::Enable() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to enable the meta logging");

    // create the network instance.
    if (network.Create(Infinit::Network) == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to create the network instance");

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
          throw reactor::Exception(elle::concurrency::scheduler(),
                          str(fmt % Hole::Descriptor.model.type));
        }
      }
  }

  void
  Hole::join()
  {
    // join the network
    Hole::Implementation->Join();
  }

  void
  Hole::leave()
  {
    // XXX
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

  void
  Hole::ready()
  {
    ELLE_LOG_SCOPE("ready");
    if (Hole::state != Hole::StateOnline)
      {
        Hole::_ready();
        Hole::state = Hole::StateOnline;
      }
  }

  void
  Hole::readyHook(boost::function<void ()> const& f)
  {
    Hole::_ready.connect(f);
  }

  ///
  /// this method returns the address of the root block i.e the origin.
  ///
  elle::Status          Hole::Origin(nucleus::proton::Address& address)
  {
    // return the address.
    address = Hole::Descriptor.root;

    return elle::Status::Ok;
  }

  ///
  /// this method stores the given block.
  ///
  elle::Status          Hole::Push(const nucleus::proton::Address& address,
                                   const nucleus::proton::Block& block)
  {
    // XXX check the block's footprint which should not exceed Extent

    // forward the request depending on the nature of the block which
    // the address indicates.
    switch (address.family)
      {
      case nucleus::proton::FamilyContentHashBlock:
        {
          const nucleus::proton::ImmutableBlock*        ib;

          // cast to an immutable block.
          ib = static_cast<const nucleus::proton::ImmutableBlock*>(&block);
          assert(dynamic_cast<const nucleus::proton::ImmutableBlock*>(
                   &block) != nullptr);

          // store the immutable block.
          Hole::Implementation->Put(address, *ib);
          break;
        }
      case nucleus::proton::FamilyPublicKeyBlock:
      case nucleus::proton::FamilyOwnerKeyBlock:
      case nucleus::proton::FamilyImprintBlock:
        {
          const nucleus::proton::MutableBlock*          mb;

          // cast to a mutable block.
          mb = static_cast<const nucleus::proton::MutableBlock*>(&block);
          assert(dynamic_cast<const nucleus::proton::MutableBlock*>(
                   &block) != nullptr);

          // store the mutable block.
          Hole::Implementation->Put(address, *mb);
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

  std::unique_ptr<nucleus::proton::Block>
  Hole::Pull(const nucleus::proton::Address& address,
             const nucleus::proton::Version& version)
  {
    ELLE_LOG_SCOPE("pull(%s, %s)", address, version);

    // Forward the request depending on the nature of the block which
    // the addres indicates.
    switch (address.family)
      {
        case nucleus::proton::FamilyContentHashBlock:
          return Hole::Implementation->Get(address);
        case nucleus::proton::FamilyPublicKeyBlock:
        case nucleus::proton::FamilyOwnerKeyBlock:
        case nucleus::proton::FamilyImprintBlock:
          return Hole::Implementation->Get(address, version);
        default:
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   elle::sprintf("unknown block family '%u'", address.family));
      }
  }

  void
  Hole::Wipe(nucleus::proton::Address const& address)
  {
    Hole::Implementation->Kill(address);
  }

}
