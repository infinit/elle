//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Cell.cc
//
// created       julien quintard   [tue jan 26 14:07:29 2010]
// updated       julien quintard   [wed jan 27 23:42:05 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Cell.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Cell::Cell():
      block(NULL)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Cell::Set(hole::Block*			block)
    {
      //
      // update the internal attributes
      //
      {
	// if the current and new blocks have the same address, do nothing.
	if (this->block == block)
	  leave();

	// release the current content.
	if (this->Destroy() == StatusError)
	  escape("unable to release the current content");

	// set the block.
	this->block = block;
      }

      //
      // increase the cache usage.
      //
      {
	// XXX
      }

      leave();
    }

    Status		Cell::Get(hole::Block*&			block)
    {
      // set the value.
      block = this->block;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Cell::Destroy()
    {
      // release the previously stored block.
      if (this->block != NULL)
	delete this->block;

      // update cache usage.
      // XXX

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the cell.
    ///
    Status		Cell::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Cell] " << std::endl;

      std::cout << alignment << shift << "[Block] "
		<< std::hex << this->block << std::endl;

      leave();
    }

  }
}
