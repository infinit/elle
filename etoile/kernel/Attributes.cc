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
// updated       julien quintard   [tue apr  6 16:34:49 2010]
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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// destructor.
    ///
    Attributes::~Attributes()
    {
      Attributes::Iterator	i;

      // until the attributes is empty.
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
    /// this method adds a trait to the attributes.
    ///
    Status		Attributes::Add(Trait*			trait)
    {
      enter();

      // add the trait to the container.
      this->container.push_back(trait);

      leave();
    }

    ///
    /// this method returns true if the name is found among the traits.
    ///
    Status		Attributes::Exist(const String&		name)
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
    Status		Attributes::Lookup(const String&	name,
					   Trait*&		trait)
    {
      Attributes::Iterator	iterator;

      enter();

      // locate the trait.
      if (this->Locate(name, &iterator) == false)
	false();

      // return the trait.
      trait = *iterator;

      true();
    }

    ///
    /// this method removes a trait from the attributes.
    ///
    Status		Attributes::Remove(const String&	name)
    {
      Attributes::Iterator	iterator;

      enter();

      // locate the trait.
      if (this->Locate(name, &iterator) == false)
	escape("the named trait does not seem to exist");

      // erase the trait.
      this->container.erase(iterator);

      leave();
    }

    ///
    /// this method returns the number of access entries in the attributes.
    ///
    Status		Attributes::Capacity(Size&		size) const
    {
      enter();

      // return the size.
      size = this->container.size();

      leave();
    }

    ///
    /// this method returns an iterator on the identified trait. this
    /// method can easily be used to test if a subject belong to the
    /// attributes: Locate(subject) returns true or false.
    ///
    /// the method returns true if the trait is found, false otherwise.
    ///
    Status		Attributes::Locate(const String&	name,
					   Attributes::Iterator* iterator)
    {
      Attributes::Iterator	i;

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
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Attributes::operator==(const Attributes& element) const
    {
      Attributes::Scoutor	i;
      Attributes::Scoutor	j;

      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the container.
      for (i = this->container.begin(), j = element.container.begin();
	   (i != this->container.end()) && (j != element.container.end());
	   i++, j++)
	{
	  // check if only one of the iterator has reached the end.
	  if ((i == this->container.end()) || (j == element.container.end()))
	    false();

	  // compare the traits.
	  if (*i != *j)
	    false();
	}

      true();
    }

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
      Attributes::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Attributes]" << std::endl;

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
    /// this method serializes the attributes object.
    ///
    Status		Attributes::Serialize(Archive&		archive) const
    {
      Attributes::Scoutor	scoutor;
      Size			size;

      enter();

      // retrieve the number of traits.
      size = this->container.size();

      // serialize the number of traits.
      if (archive.Serialize(size) == StatusError)
	escape("unable to serialize the number of traits");

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
    /// this method extracts the attributes object.
    ///
    Status		Attributes::Extract(Archive&		archive)
    {
      Size		size;
      Index		i;

      enter();

      // extract the number of traits.
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
