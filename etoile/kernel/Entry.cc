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
// updated       julien quintard   [fri may 28 17:47:57 2010]
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
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Entry::operator==(const Entry&	element) const
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
    /// this macro-function call generates the object.
    ///
    embed(Entry, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a entry.
    ///
    elle::Status	Entry::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Entry]" << std::endl;

      // dump the name.
      std::cout << alignment << elle::Dumpable::Shift << "[Name] "
		<< this->name << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the token");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the entry object.
    ///
    elle::Status	Entry::Serialize(elle::Archive&		archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->name,
			    this->address) == elle::StatusError)
	escape("unable to serialize the entry");

      leave();
    }

    ///
    /// this method extracts the entry object.
    ///
    elle::Status	Entry::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->name,
			  this->address) == elle::StatusError)
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
