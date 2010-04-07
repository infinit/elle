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
// updated       julien quintard   [fri apr  2 13:51:03 2010]
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
      if (this->address.Dump(margin + 4) == StatusError)
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

  }
}
