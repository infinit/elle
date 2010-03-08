//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/path/Item.cc
//
// created       julien quintard   [fri aug  7 21:44:49 2009]
// updated       julien quintard   [wed mar  3 23:48:32 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Item.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor, especially used for creating the root item
    /// which neither has a name nor a parent directory.
    ///
    Item::Item():
      directory(NULL),
      data(NULL)
    {
    }

    ///
    /// the normal constructor.
    ///
    Item::Item(const String&					name,
	       Item*						directory):
      name(name),
      directory(directory),
      data(NULL)
    {
    }

    ///
    /// the destructor.
    ///
    Item::~Item()
    {
      // XXX
      // purge the object.
      //this->Purge();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method tries to resolve a name within the item scope.
    ///
    Status		Item::Resolve(const String&		name,
				      Item*&			item)
    {
      Item::Scoutor	scoutor;

      enter();

      // stop if there is no more entries.
      if (this->data == NULL)
	false();

      // look up the element in the current item .. stop if not present.
      if ((scoutor = this->data->find(name)) == this->data->end())
	false();

      // set the resolved item.
      item = scoutor->second;

      true();
    }

    ///
    /// XXX
    ///
    Status		Item::Update(const String&		name,
				     const hole::Address&	address)
    {
      std::pair<Item::Iterator, bool>	result;
      Item::Scoutor			scoutor;
      Item*				item;

      enter(instance(item));

      // if there is no data yet, create it.
      if (this->data == NULL)
	{
	  this->data = new Item::Container;
	}
      else
	{
	  // otherwise, look up the element in the current item.
	  if ((scoutor = this->data->find(name)) != this->data->end())
	    {
	      // if found, update it with the new address.
	      scoutor->second->address = address;

	      leave();
	    }
	}

      // create a new item.
      item = new Item(name, this);

      // set its address.
      item->address = address;

      // insert it.
      result =
	this->data->insert(Item::Value(name, item));

      // check the result.
      if (result.second == false)
	escape("unable to insert the new item");

      // stop tracking.
      waive(item);

      leave();
    }

    ///
    /// XXX
    ///
    Status		Item::Destroy()
    {
      enter();

      // XXX

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the item in a recursive way.
    ///
    Status		Item::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Item]" << std::endl;

      // dump the attributes.
      std::cout << alignment << shift << "[Name] "
		<< this->name << std::endl;

      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the address");

      std::cout << alignment << shift << "[Directory] "
		<< std::hex << this->directory << std::endl;

      // dump the data, if there is any.
      if (this->data != NULL)
	{
	  Item::Scoutor	scoutor;

	  std::cout << alignment << shift << "[Data]" << std::endl;

	  // recursively dump all the sub-items.
	  for (scoutor = this->data->begin();
	       scoutor != this->data->end();
	       scoutor++)
	    {
	      // dump the sub-item.
	      if (scoutor->second->Dump(margin + 4) == StatusError)
		escape("unable to dump the sub-item");
	    }
	}

      leave();
    }

  }
}
