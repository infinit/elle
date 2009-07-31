//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Block.cc
//
// created       julien quintard   [mon feb 16 17:08:04 2009]
// updated       julien quintard   [thu jul 30 20:19:46 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Block.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Block::Block()
    {
    }

    ///
    /// this method reinitializes the object.
    ///
    Block::~Block()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method validates the block by comparing the current address
    /// with the given address.
    ///
    /// in practice, this method should be called with the address
    /// used when requesting the block to make sure the block received
    /// matches the one requested.
    ///
    Status		Block::Validate(Address&		address)
    {
      if (this->address != address)
	false();

      true();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		Block::Dump(Natural32			margin)
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[Block]" << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    Status		Block::Serialize(Archive&		archive) const
    {
      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		Block::Extract(Archive&		archive)
    {
      leave();
    }

  }
}
