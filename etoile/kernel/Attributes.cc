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
// updated       julien quintard   [fri may 28 17:45:03 2010]
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
    elle::Status	Attributes::Add(Trait*			trait)
    {
      enter();

      // add the trait in the range.
      if (this->range.Add(trait) == elle::StatusError)
	escape("unable to add the trait in the range");

      leave();
    }

    ///
    /// this method tests if the given name exists.
    ///
    elle::Status	Attributes::Exist(const elle::String&		name)
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
    elle::Status	Attributes::Lookup(const elle::String&	name,
					   Trait*&		trait)
    {
      enter();

      // look in the range.
      if (this->range.Lookup(name, trait) == elle::StatusError)
	escape("unable to retrieve the trait");

      leave();
    }

    ///
    /// this method returns a subset of the attributes delimited by the given
    /// index and size.
    ///
    elle::Status	Attributes::Consult(const Index&	index,
					    const Size&		size,
					    Range<Trait>&	range)
      const
    {
      Range<Trait>::Scoutor	scoutor;
      Index			i;

      enter();

      // first detach the data from the range.
      if (range.Detach() == elle::StatusError)
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
	      if (range.Add(trait) == elle::StatusError)
		escape("unable to add the trait to the range");
	    }
	}

      leave();
    }

    ///
    /// this method updates an existing trait's value.
    ///
    elle::Status	Attributes::Update(const elle::String&	name,
					   const elle::String&	value)
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
    elle::Status	Attributes::Remove(const elle::String&	name)
    {
      enter();

      // remove the trait from the range.
      if (this->range.Remove(name) == elle::StatusError)
	escape("unable to remove the trait");

      leave();
    }

    ///
    /// this method returns the size of the attributes.
    ///
    elle::Status	Attributes::Capacity(Size&		size) const
    {
      enter();

      // look at the size of the range.
      if (this->range.Capacity(size) == elle::StatusError)
	escape("unable to retrieve the range size");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Attributes, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a attributes.
    ///
    elle::Status	Attributes::Dump(elle::Natural32	margin) const
    {
      elle::String		alignment(margin, ' ');
      Range<Trait>::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Attributes]" << std::endl;

      // dump the range.
      if (this->range.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the range");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the attributes object.
    ///
    elle::Status	Attributes::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the range.
      if (archive.Serialize(this->range) == elle::StatusError)
	escape("unable to serialize the range");

      leave();
    }

    ///
    /// this method extracts the attributes object.
    ///
    elle::Status	Attributes::Extract(elle::Archive&	archive)
    {
      enter();

      // extract the range.
      if (archive.Extract(this->range) == elle::StatusError)
	escape("unable to extract the range");

      leave();
    }

  }
}
