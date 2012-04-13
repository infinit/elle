//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [thu apr  1 22:00:03 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Entry.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines an unexisting entry.
    ///
    const Entry                 Entry::Null;

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
    Entry::Entry(const elle::String&                            name,
                 const proton::Address&                         address):
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
    elle::Boolean       Entry::operator==(const Entry&  element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::Status::True;

      // compare the attributes.
      if ((this->name != element.name) ||
          (this->address != element.address))
        return elle::Status::False;

      return elle::Status::True;
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
    elle::Status        Entry::Dump(elle::Natural32             margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Entry]" << std::endl;

      // dump the name.
      std::cout << alignment << elle::Dumpable::Shift << "[Name] "
                << this->name << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the token");

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the entry object.
    ///
    elle::Status        Entry::Serialize(elle::Archive&         archive) const
    {
      // serialize the attributes.
      if (archive.Serialize(this->name,
                            this->address) == elle::Status::Error)
        escape("unable to serialize the entry");

      return elle::Status::Ok;
    }

    ///
    /// this method extracts the entry object.
    ///
    elle::Status        Entry::Extract(elle::Archive&           archive)
    {
      // extract the attributes.
      if (archive.Extract(this->name,
                          this->address) == elle::Status::Error)
        escape("unable to extract the entry");

      return elle::Status::Ok;
    }

//
// ---------- rangeable -------------------------------------------------------
//

    ///
    /// this method returns the symbol of an entry i.e the name.
    ///
    elle::String&       Entry::Symbol()
    {
      return (this->name);
    }

  }
}
