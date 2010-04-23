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
// updated       julien quintard   [thu apr 22 21:49:27 2010]
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

      // add the trait in the range.
      if (this->range.Add(trait) == StatusError)
	escape("unable to add the trait in the range");

      leave();
    }

    ///
    /// this method tests if the given name exists.
    ///
    Status		Attributes::Exist(const String&		name)
    {
      enter();

      // test.
      if (this->range.Exist(name) == false)
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

      // look in the range.
      if (this->range.Lookup(name, trait) == StatusError)
	escape("unable to retrieve the trait");

      leave();
    }

    ///
    /// this method returns a subset of the attributes delimited by the given
    /// index and size.
    ///
    Status		Attributes::Consult(const Index&	index,
					    const Size&		size,
					    Range<Trait>&	range)
      const
    {
      Range<Trait>::Scoutor	scoutor;
      Index			i;

      enter();

      // first detach the data from the range.
      if (range.Detach() == StatusError)
	escape("unable to detach the data from the range");

      // go through the attributes entries.
      for (scoutor = this->range.container.begin(), i = 0;
	   scoutor != this->range.container.end();
	   scoutor++, i++)
	{
	  Trait*	trait = *scoutor;

	  // if this trait lies in the selected set [index, index + size[
	  if ((i >= index) && (i < (index + size)))
	    {
	      // add the trait to the range.
	      if (range.Add(trait) == StatusError)
		escape("unable to add the trait to the range");
	    }
	}

      leave();
    }

    ///
    /// this method updates an existing trait's value.
    ///
    Status		Attributes::Update(const String&	name,
					   const String&	value)
    {
      Range<Trait>::Iterator	iterator;

      enter();

      // locate the trait.
      if (this->range.Locate(name, &iterator) == false)
	escape("unable to locate the named trait");

      // update the value.
      (*iterator)->value = value;

      leave();
    }

    ///
    /// this method removes the given trait.
    ///
    Status		Attributes::Remove(const String&	name)
    {
      enter();

      // remove the trait from the range.
      if (this->range.Remove(name) == StatusError)
	escape("unable to remove the trait");

      leave();
    }

    ///
    /// this method returns the size of the attributes.
    ///
    Status		Attributes::Capacity(Size&		size) const
    {
      enter();

      // look at the size of the range.
      if (this->range.Capacity(size) == StatusError)
	escape("unable to retrieve the range size");

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
      Range<Trait>::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Attributes]" << std::endl;

      // dump the range.
      if (this->range.Dump(margin + 2) == StatusError)
	escape("unable to dump the range");

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

      // serialize the range.
      if (archive.Serialize(this->range) == StatusError)
	escape("unable to serialize the range");

      leave();
    }

    ///
    /// this method extracts the attributes object.
    ///
    Status		Attributes::Extract(Archive&		archive)
    {
      enter();

      // extract the range.
      if (archive.Extract(this->range) == StatusError)
	escape("unable to extract the range");

      leave();
    }

  }
}
