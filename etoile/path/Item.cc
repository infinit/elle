//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Item.cc
//
// created       julien quintard   [fri aug  7 21:44:49 2009]
// updated       julien quintard   [mon may  3 20:54:16 2010]
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
    Item::Item(const Slice&					name,
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
    elle::Status	Item::Resolve(const Slice&		name,
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
    elle::Status	Item::Update(const Slice&		name,
				     const hole::Address&	address)
    {
      std::pair<Item::Iterator, elle::Boolean>	result;
      Item::Scoutor				scoutor;
      Item*					item;

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
    elle::Status		Item::Destroy()
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
    elle::Status	Item::Dump(const elle::Natural32	margin)
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Item]" << std::endl;

      // dump the attributes.
      std::cout << alignment << elle::Dumpable::Shift << "[Name] "
		<< this->name << std::endl;

      if (this->address.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the address");

      std::cout << alignment << elle::Dumpable::Shift << "[Directory] "
		<< std::hex << this->directory << std::endl;

      // dump the data, if there is any.
      if (this->data != NULL)
	{
	  Item::Scoutor	scoutor;

	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Data]" << std::endl;

	  // recursively dump all the sub-items.
	  for (scoutor = this->data->begin();
	       scoutor != this->data->end();
	       scoutor++)
	    {
	      // dump the sub-item.
	      if (scoutor->second->Dump(margin + 4) == elle::StatusError)
		escape("unable to dump the sub-item");
	    }
	}

      leave();
    }

  }
}
