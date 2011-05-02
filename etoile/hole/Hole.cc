//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/hole/Hole.cc
//
// created       julien quintard   [sun aug  9 16:47:38 2009]
// updated       julien quintard   [mon may  2 12:25:32 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Hole.hh>

#include <etoile/kernel/Object.hh>
#include <etoile/kernel/Access.hh>

namespace etoile
{
  namespace hole
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method takes a live block and stores its data into the storage
    /// layer.
    ///
    elle::Status	Hole::Put(const Address&		address,
				  const Block*			block)
    {
      enter();

      // store the block in the given universe.
      if (block->Store(address) == elle::StatusError)
	escape("unable to store the block");

      leave();
    }

    ///
    /// this method retrieves data from the storage layer and extracts it
    /// into a live block.
    ///
    /// \todo XXX this method should call Gather() for mutable blocks and
    ///    handle the protocol such that the DHT does not care about
    ///    things such as mutable/immutable distinctions etc.
    /// \todo XXX note that although it would be nice to make the DHT not care
    ///    about the types of blocks and their validity its going to be
    ///    difficult. for example this method verifies the validaity at the
    ///    end. if this test fails, this method would have to re-ask the
    ///    DHT. however, since the DHT has no context for this operation
    ///    failure, it is likely to return the exact same block :(
    /// \todo XXX therefore the simplest way would be for the storage layer
    ///    to actually do everything that is here meaning extracting and
    ///    verifying. the storage layer (Hole) should be able to understand
    ///    Address and Block!
    ///
    elle::Status	Hole::Get(const Address&		address,
				  Block*&			block)
    {
      enter();

      // does the block exist.
      if (block->Exist(address) == elle::StatusFalse)
	escape("the block does not seem to exist");

      // load the block.
      if (block->Load(address) == elle::StatusError)
	escape("unable to load the block");

      leave();
    }

    ///
    /// XXX
    ///
    /// \todo XXX this method should ask the storage nodes to destroy the
    ///   data, whose should challenge our clients, proving that we are
    ///   the owner.
    ///
    elle::Status	Hole::Erase(const Address&		address)
    {
      hole::Block	block;

      enter();

      // erase the block.
      if (block.Erase(address) == elle::StatusError)
	escape("unable to erase the block");

      leave();
    }

  }
}
