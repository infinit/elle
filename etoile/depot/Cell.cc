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
// updated       julien quintard   [mon may  3 20:50:49 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Cell.hh>
#include <etoile/depot/Repository.hh>

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
    elle::Status	Cell::Set(hole::Block*			block)
    {
      enter();

      //
      // update the internal attributes
      //
      {
	// if the current and new blocks have the same address, do nothing.
	if (this->block == block)
	  leave();

	// release the current content.
	if (this->Destroy() == elle::StatusError)
	  escape("unable to release the current content");

	// set the block.
	this->block = block;
      }

      //
      // increase the cache usage.
      //
      {
	elle::Natural32	size;

	// retrieve the block's memory imprint.
	if (this->block->Imprint(size) == elle::StatusError)
	  escape("unable to retrieve the object's memory imprint");

	// update the cache usage.
	Repository::Cache::Size += (elle::Natural64)size;
      }

      leave();
    }

    ///
    /// this method returns the block's content.
    ///
    elle::Status	Cell::Get(hole::Block*&			block)
    {
      enter();

      // set the value.
      block = this->block;

      leave();
    }

    ///
    /// this method releases the resources allocated by the cell and
    /// therefore updates the cache usage since one block is being released.
    ///
    elle::Status	Cell::Destroy()
    {
      elle::Natural32	size;

      enter();

      // release the previously stored block.
      if (this->block != NULL)
	{
	  // retrieve the block's memory imprint.
	  if (this->block->Imprint(size) == elle::StatusError)
	    escape("unable to retrieve the object's memory imprint");

	  // update the cache usage.
	  Repository::Cache::Size -= (elle::Natural64)size;

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
    elle::Status	Cell::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');
      elle::Natural32	size;

      enter();

      // retrieve the block's memory imprint.
      if (this->block->Imprint(size) == elle::StatusError)
	escape("unable to retrieve the object's memory imprint");

      std::cout << alignment << "[Cell] " << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << "[Block] "
		<< std::hex << this->block << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << "[Size] "
		<< std::hex << size << std::endl;

      leave();
    }

  }
}
