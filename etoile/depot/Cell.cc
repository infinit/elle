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
// updated       julien quintard   [sat jan 30 04:10:59 2010]
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
    /// this method sets for the first time the cell or updates it.
    ///
    /// note that if the pointer of the current and new blocks are identical,
    /// nothing is done as the block has already been modified in place.
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
	Natural32	size;

	// retrieve the block's memory imprint.
	if (this->block->Imprint(size) == StatusError)
	  escape("unable to retrieve the object's memory imprint");

	// update the cache usage.
	Repository::Cache::Size += (Natural64)size;
      }

      leave();
    }

    ///
    /// this method returns the block's content.
    ///
    Status		Cell::Get(hole::Block*&			block)
    {
      // set the value.
      block = this->block;

      leave();
    }

    ///
    /// this method releases the resources allocated by the cell and
    /// therefore updates the cache usage since one block is being released.
    ///
    Status		Cell::Destroy()
    {
      Natural32		size;

      // release the previously stored block.
      if (this->block != NULL)
	{
	  // retrieve the block's memory imprint.
	  if (this->block->Imprint(size) == StatusError)
	    escape("unable to retrieve the object's memory imprint");

	  // update the cache usage.
	  Repository::Cache::Size -= (Natural64)size;

	  // delete the block.
	  delete this->block;
	}

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
      Natural32		size;

      // retrieve the block's memory imprint.
      if (this->block->Imprint(size) == StatusError)
	escape("unable to retrieve the object's memory imprint");

      std::cout << alignment << "[Cell] " << std::endl;

      std::cout << alignment << shift << "[Block] "
		<< std::hex << this->block << std::endl;

      std::cout << alignment << shift << "[Size] "
		<< std::hex << size << std::endl;

      leave();
    }

  }
}
