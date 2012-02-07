//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [tue apr 13 15:27:20 2010]
//

//
// ---------- includes --------------------------------------------------------
//

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
  /// this variable contains the device passport.
  ///
  lune::Passport                Hole::Passport;

  ///
  /// this variable holds the hole implementation.
  ///
  Holeable*                     Hole::Implementation = NULL;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the hole by allocating and initializing
  /// the implementation.
  ///
  elle::Status          Hole::Initialize()
  {
    nucleus::Network    network;

    ;

    // disable the meta logging.
    if (elle::Meta::Disable() == elle::StatusError)
      escape("unable to disable the meta logging");

    //
    // retrieve the descriptor.
    //
    {
      // does the network exist.
      if (Hole::Descriptor.Exist(Infinit::Network) == elle::StatusFalse)
        escape("this network does not seem to exist");

      // load the descriptor.
      if (Hole::Descriptor.Load(Infinit::Network) == elle::StatusError)
        escape("unable to load the descriptor");

      // pull the attributes.
      if (Hole::Descriptor.Pull() == elle::StatusError)
        escape("unable to pull the descriptor's attributes");

      // validate the descriptor.
      if (Hole::Descriptor.Validate(Infinit::Authority) == elle::StatusError)
        escape("unable to validate the descriptor");
    }

    //
    // retrieve the passport.
    //
    {
      // does the network exist.
      if (Hole::Passport.Exist() == elle::StatusFalse)
        escape("the device passport does not seem to exist");

      // load the passport.
      if (Hole::Passport.Load() == elle::StatusError)
        escape("unable to load the passport");

      // validate the passport.
      if (Hole::Passport.Validate(Infinit::Authority) == elle::StatusError)
        escape("unable to validate the passport");
    }

    // enable the meta logging.
    if (elle::Meta::Enable() == elle::StatusError)
      escape("unable to enable the meta logging");

    // create the network instance.
    if (network.Create(Infinit::Network) == elle::StatusError)
      escape("unable to create the network instance");

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
        escape("unknown or not-yet-supported model '%u'",
               Hole::Descriptor.model.type);
      }

    // join the network
    if (Hole::Implementation->Join() == elle::StatusError)
      escape("unable to join the network");

    return elle::StatusOk;
  }

  ///
  /// this method cleans the hole.
  ///
  /// the components are recycled just to make sure the memory is
  /// released before the Meta allocator terminates.
  ///
  elle::Status          Hole::Clean()
  {
    ;

    // leave the network
    if (Hole::Implementation->Leave() == elle::StatusError)
      escape("unable to leave the network");

    // delete the implementation.
    delete Hole::Implementation;

    return elle::StatusOk;
  }

  ///
  /// this method returns the address of the root block i.e the origin.
  ///
  elle::Status          Hole::Origin(nucleus::Address&          address)
  {
    ;

    // return the address.
    address = Hole::Descriptor.root;

    return elle::StatusOk;
  }

  ///
  /// this method stores the given block.
  ///
  elle::Status          Hole::Push(const nucleus::Address&      address,
                                   const nucleus::Block&        block)
  {
    ;

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
          if (Hole::Implementation->Put(address, *ib) == elle::StatusError)
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
          if (Hole::Implementation->Put(address, *mb) == elle::StatusError)
            escape("unable to put the block");

          break;
        }
      default:
        {
          escape("unknown block family '%u'",
                 address.family);
        }
      }

    return elle::StatusOk;
  }

  ///
  /// this method returns the block associated with the given address.
  ///
  elle::Status          Hole::Pull(const nucleus::Address&      address,
                                   const nucleus::Version&      version,
                                   nucleus::Block&              block)
  {
    ;

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
          if (Hole::Implementation->Get(address, *ib) == elle::StatusError)
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
                                        *mb) == elle::StatusError)
            escape("unable to get the block");

          break;
        }
      default:
        {
          escape("unknown block family '%u'",
                 address.family);
        }
      }

    return elle::StatusOk;
  }

  ///
  /// this method removes the block associated with the given address.
  ///
  elle::Status          Hole::Wipe(const nucleus::Address&      address)
  {
    ;

    // forward the kill request to the implementation.
    if (Hole::Implementation->Kill(address) == elle::StatusError)
      escape("unable to erase the block");

    return elle::StatusOk;
  }

}
