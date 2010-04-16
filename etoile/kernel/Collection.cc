//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Collection.cc
//
// created       julien quintard   [wed mar 31 23:36:12 2010]
// updated       julien quintard   [thu apr 15 16:31:09 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Collection.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// destructor.
    ///
    Collection::~Collection()
    {
      Collection::Iterator	i;

      // until the collection is empty.
      while (this->container.empty() == false)
	{
	  Trait*	trait = this->container.front();

	  // remove the first element.
	  this->container.pop_front();

	  // delete the trait.
	  delete trait;
	}
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds an item to the collection.
    ///
    Status		Collection::Add(Trait*				trait)
    {
      enter();

      // add the trait to the container.
      this->container.push_back(trait);

      leave();
    }

    ///
    /// this method returns true if the collection contains the given name.
    ///
    Status		Collection::Exist(const String&		name)
    {
      enter();

      // try to locate the trait.
      if (this->Locate(name) != StatusTrue)
	false();

      true();
    }

    ///
    /// this method removes the identified trait.
    ///
    /// the method returns true if the trait is found, false otherwise.
    ///
    Status		Collection::Lookup(const String&	name,
					   Trait*&		trait)
    {
      Collection::Iterator	iterator;

      enter();

      // locate the trait.
      if (this->Locate(name, &iterator) == false)
	false();

      // return the trait.
      trait = *iterator;

      true();
    }

    ///
    /// this method removes a trait from the collection.
    ///
    Status		Collection::Remove(const String&	name)
    {
      Collection::Iterator	iterator;

      enter();

      // locate the trait.
      if (this->Locate(name, &iterator) == false)
	escape("this name does not seem to be present in this collection");

      // delete the trait.
      delete *iterator;

      // erase the trait.
      this->container.erase(iterator);

      leave();
    }

    ///
    /// this method returns the number of entries in the collection.
    ///
    Status		Collection::Capacity(Size&		size) const
    {
      enter();

      // return the size.
      size = this->container.size();

      leave();
    }

    ///
    /// this method returns an iterator on the identified trait. this
    /// method can easily be used to test if a name belong to the collection:
    /// Locate(name) returns true or false.
    ///
    /// the method returns true if the trait is found, false otherwise.
    ///
    Status		Collection::Locate(const String&	name,
					   Collection::Iterator* iterator)
    {
      Collection::Iterator	i;

      enter();

      // go through the container.
      for (i = this->container.begin();
	   i != this->container.end();
	   i++)
	{
	  Trait*	trait = *i;

	  if (trait->name == name)
	    {
	      // return the iterator, if wanted.
	      if (iterator != NULL)
		*iterator = i;

	      true();
	    }
	}

      false();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a collection.
    ///
    Status		Collection::Dump(Natural32		margin) const
    {
      String			alignment(margin, ' ');
      Collection::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Collection]" << std::endl;

      // dump every trait
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Trait*	trait = *scoutor;

	  // dump the trait.
	  if (trait->Dump(margin + 2) == StatusError)
	    escape("unable to dump the trait");
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the collection object.
    ///
    Status		Collection::Serialize(Archive&		archive) const
    {
      Collection::Scoutor	scoutor;
      Size			size;

      enter();

      // retrieve the number of entries.
      size = this->container.size();

      // serialize the number of entries.
      if (archive.Serialize(size) == StatusError)
	escape("unable to serialize the number of entries");

      // serialize every trait.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Trait*	trait = *scoutor;

	  // serialize the trait.
	  if (archive.Serialize(*trait) == StatusError)
	    escape("unable to serialize the trait");
	}

      leave();
    }

    ///
    /// this method extracts the collection object.
    ///
    Status		Collection::Extract(Archive&		archive)
    {
      Size		size;
      Index		i;

      enter();

      // extract the number of entries.
      if (archive.Extract(size) == StatusError)
	escape("unable to extract the number of trait");

      // extract every trait.
      for (i = 0; i < size; i++)
	{
	  Trait*	trait;

	  enter(instance(trait));

	  // allocate a new trait.
	  trait = new Trait;

	  // extract the trait.
	  if (archive.Extract(*trait) == StatusError)
	    escape("unable to extract the trait");

	  // add the trait to the container.
	  this->container.push_back(trait);

	  // stop tracking the trait.
	  waive(trait);

	  release();
	}

      leave();
    }

  }
}
