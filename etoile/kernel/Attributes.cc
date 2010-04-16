//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Attributes.cc
//
// created       julien quintard   [wed mar 31 23:36:12 2010]
// updated       julien quintard   [thu apr 15 16:40:11 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Attributes.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds the given entry to the attributes.
    ///
    Status		Attributes::Add(Trait*			trait)
    {
      enter();

      // add the trait in the collection.
      if (this->collection.Add(trait) == StatusError)
	escape("unable to add the trait in the collection");

      leave();
    }

    ///
    /// this method returns true if the name exists in the attributes.
    ///
    Status		Attributes::Exist(const String&		name)
    {
      enter();

      // look in the collection.
      if (this->collection.Exist(name) != StatusTrue)
	false();

      true();
    }

    ///
    /// this method returns the trait corresponding to the given name.
    ///
    Status		Attributes::Lookup(const String&	name,
					   Trait*&		trait)
    {
      enter();

      // look in the collection.
      if (this->collection.Lookup(name, trait) == StatusError)
	escape("unable to retrieve the trait");

      leave();
    }

    ///
    /// this method returns a subset of the attributes delimited by the given
    /// index and size.
    ///
    Status		Attributes::Consult(const Index&	index,
					    const Size&		size,
					    Collection&		collection)
      const
    {
      Collection::Scoutor	scoutor;
      Index			i;

      enter();

      // go through the attributes entries.
      for (scoutor = this->collection.container.begin(), i = 0;
	   scoutor != this->collection.container.end();
	   scoutor++, i++)
	{
	  Trait*	trait = *scoutor;

	  // if this trait lies in the selected set [index, index + size[
	  if ((i >= index) && (i < (index + size)))
	    {
	      Trait*	t;

	      enter(instance(t));

	      // allocate a new trait.
	      t = new Trait(*trait);

	      // add the new trait to the collection.
	      if (collection.Add(t) == StatusError)
		escape("unable to add the name to the container");

	      // waive.
	      waive(t);

	      release();
	    }
	}

      leave();
    }

    ///
    /// this method removes the given trait.
    ///
    Status		Attributes::Remove(const String&	name)
    {
      enter();

      // remove the trait from the collection.
      if (this->collection.Remove(name) == StatusError)
	escape("unable to remove the trait");

      leave();
    }

    ///
    /// this method returns the size of the attributes.
    ///
    Status		Attributes::Capacity(Size&		size) const
    {
      enter();

      // look at the size of the collection.
      if (this->collection.Capacity(size) == StatusError)
	escape("unable to retrieve the collection size");

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Attributes);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a attributes.
    ///
    Status		Attributes::Dump(Natural32		margin) const
    {
      String			alignment(margin, ' ');
      Collection::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Attributes]" << std::endl;

      // dump the collection.
      if (this->collection.Dump(margin + 2) == StatusError)
	escape("unable to dump the collection");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the attributes object.
    ///
    Status		Attributes::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the collection.
      if (archive.Serialize(this->collection) == StatusError)
	escape("unable to serialize the collection");

      leave();
    }

    ///
    /// this method extracts the attributes object.
    ///
    Status		Attributes::Extract(Archive&		archive)
    {
      enter();

      // extract the collection.
      if (archive.Extract(this->collection) == StatusError)
	escape("unable to extract the collection");

      leave();
    }

  }
}
