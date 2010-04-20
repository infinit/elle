//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Entry.cc
//
// created       julien quintard   [thu apr  1 22:00:03 2010]
// updated       julien quintard   [mon apr 19 13:52:23 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Entry.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines an unexisting entry.
    ///
    const Entry			Entry::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// empty constructor.
    ///
    Entry::Entry()
    {
    }

    ///
    /// default constructor.
    ///
    Entry::Entry(const path::Slice&				name,
		 const hole::Address&				address):
      name(name),
      address(address)
    {
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Entry::operator==(const Entry&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the attributes.
      if ((this->name != element.name) ||
	  (this->address != element.address))
	false();

      true();
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Entry);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a entry.
    ///
    Status		Entry::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Entry]" << std::endl;

      // dump the name.
      std::cout << alignment << Dumpable::Shift << "[Name] "
		<< this->name << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the token");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the entry object.
    ///
    Status		Entry::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->name,
			    this->address) == StatusError)
	escape("unable to serialize the entry");

      leave();
    }

    ///
    /// this method extracts the entry object.
    ///
    Status		Entry::Extract(Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->name,
			  this->address) == StatusError)
	escape("unable to extract the entry");

      leave();
    }

//
// ---------- rangeable -------------------------------------------------------
//

    ///
    /// this method returns the symbol of an entry i.e the name.
    ///
    path::Slice&	Entry::Symbol()
    {
      return (this->name);
    }

  }
}
