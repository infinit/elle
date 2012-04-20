//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [wed mar 31 23:36:12 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Attributes.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds the given entry to the attributes.
    ///
    elle::Status        Attributes::Add(Trait*                  trait)
    {
      // check that the trait's name is non-empty.
      if (trait->name.empty() == true)
        escape("unable to create an empty-named trait in the attributes");

      // add the trait in the range.
      if (this->range.Add(trait) == elle::Status::Error)
        escape("unable to add the trait in the range");

      return elle::Status::Ok;
    }

    ///
    /// this method tests if the given name exists.
    ///
    elle::Status        Attributes::Exist(const elle::String&   name) const
    {
      // test.
      if (this->range.Exist(name) == false)
        return elle::Status::False;

      return elle::Status::True;
    }

    ///
    /// this method returns the trait corresponding to the given name.
    ///
    elle::Status        Attributes::Lookup(const elle::String&  name,
                                           Trait*&              trait) const
    {
      // look in the range.
      if (this->range.Lookup(name, trait) == elle::Status::Error)
        escape("unable to retrieve the trait");

      return elle::Status::Ok;
    }

    ///
    /// this method returns a subset of the attributes delimited by the given
    /// index and size.
    ///
    elle::Status        Attributes::Consult(const Index&        index,
                                            const Size&         size,
                                            Range<Trait>&       range) const
    {
      Range<Trait>::Scoutor     scoutor;
      Index                     i;

      // first detach the data from the range.
      if (range.Detach() == elle::Status::Error)
        escape("unable to detach the data from the range");

      // go through the attributes entries.
      for (scoutor = this->range.container.begin(), i = 0;
           scoutor != this->range.container.end();
           scoutor++, i++)
        {
          Trait*        trait = *scoutor;

          // if this trait lies in the selected set [index, index + size[
          if ((i >= index) && (i < (index + size)))
            {
              // add the trait to the range.
              if (range.Add(trait) == elle::Status::Error)
                escape("unable to add the trait to the range");
            }
        }

      return elle::Status::Ok;
    }

    ///
    /// this method updates an existing trait's value.
    ///
    elle::Status        Attributes::Update(const elle::String&  name,
                                           const elle::String&  value)
    {
      Range<Trait>::Iterator    iterator;

      // locate the trait.
      if (this->range.Locate(name, iterator) == false)
        escape("unable to locate the named trait");

      // update the value.
      (*iterator)->value = value;

      return elle::Status::Ok;
    }

    ///
    /// this method removes the given trait.
    ///
    elle::Status        Attributes::Remove(const elle::String&  name)
    {
      // remove the trait from the range.
      if (this->range.Remove(name) == elle::Status::Error)
        escape("unable to remove the trait");

      return elle::Status::Ok;
    }

    ///
    /// this method returns the size of the attributes.
    ///
    elle::Status        Attributes::Capacity(Size&              size) const
    {
      // look at the size of the range.
      if (this->range.Capacity(size) == elle::Status::Error)
        escape("unable to retrieve the range size");

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Attributes::operator==(const Attributes& element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::Status::True;

      // compare the ranges.
      if (this->range != element.range)
        return elle::Status::False;

      return elle::Status::True;
    }

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
    elle::Status        Attributes::Dump(elle::Natural32        margin) const
    {
      elle::String              alignment(margin, ' ');
      Range<Trait>::Scoutor     scoutor;

      std::cout << alignment << "[Attributes]" << std::endl;

      // dump the range.
      if (this->range.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the range");

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the attributes object.
    ///
    //elle::Status        Attributes::Serialize(elle::Archive&    archive) const
    //{
    //  // serialize the range.
    //  if (archive.Serialize(this->range) == elle::Status::Error)
    //    escape("unable to serialize the range");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the attributes object.
    /////
    //elle::Status        Attributes::Extract(elle::Archive&      archive)
    //{
    //  // extract the range.
    //  if (archive.Extract(this->range) == elle::Status::Error)
    //    escape("unable to extract the range");

    //  return elle::Status::Ok;
    //}

  }
}
