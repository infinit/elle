//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Bucket.cc
//
// created       julien quintard   [mon apr  5 21:33:25 2010]
// updated       julien quintard   [fri may 13 17:38:52 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Bucket.hh>

namespace etoile
{
  namespace journal
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// destructor.
    ///
    Bucket::~Bucket()
    {
      Bucket::Scoutor	scoutor;

      // delete every item.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Item*		item = *scoutor;

	  // delete the item.
	  delete item;
	}
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method records a block to be pushed to the hole.
    ///
    elle::Status	Bucket::Push(const nucleus::Address&	address,
				     nucleus::Block*		block)
    {
      Item*		item;

      enter(instance(item));

      // check the address.
      if (address == nucleus::Address::Null)
	escape("a block cannot be pushed with a null address");

      // allocate and create an item.
      item = new Item(address, block);

      // add the item to the container.
      this->container.push_back(item);

      // stop tracking.
      waive(item);

      leave();
    }

    ///
    /// this method records a block to be poped from the hole.
    ///
    elle::Status	Bucket::Destroy(const nucleus::Address&	address)
    {
      Item*		item;

      enter(instance(item));

      // check the address.
      if (address == nucleus::Address::Null)
	escape("a null-address-referenced block cannot be destroyed");

      // allocate and create an item.
      item = new Item(address);

      // add the item to the container.
      this->container.push_back(item);

      // stop tracking.
      waive(item);

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the bucket.
    ///
    elle::Status	Bucket::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');
      Bucket::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Bucket] " << std::endl;

      // go through the items.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Item*		item = *scoutor;

	  // dump the item.
	  if (item->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the item");
	}

      leave();
    }

  }
}
