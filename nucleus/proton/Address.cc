#include <elle/standalone/Log.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Address.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable can easily be used for comparing with invalid addresses.
    ///
    const Address               Address::Null;

    ///
    /// this variable is similar to Null except that it is constructed
    /// so as not to be considered "empty". this constant is particularly
    /// useful when one wants to know the footprint of such an Address type.
    ///
    Address                     Address::Any;

    ///
    /// this variable is an alias of Any.
    ///
    Address&                    Address::Some = Address::Any;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the address system.
    ///
    elle::Status        Address::Initialize()
    {
      // disable the meta logging.
      if (elle::radix::Meta::Disable() == elle::Status::Error)
        escape("unable to disable the meta logging");

      // create the any address with default meaningless values.
      if (Address::Any.Create(
            Address::Any.family, Address::Any.component,
            Address::Any.family,
            Address::Any.component) ==
          elle::Status::Error)
        escape("unable to create the any address");

    // enable the meta logging.
    if (elle::radix::Meta::Enable() == elle::Status::Error)
      escape("unable to enable the meta logging");

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the address system.
    ///
    elle::Status        Address::Clean()
    {
      // nothing to do.

      return elle::Status::Ok;
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Address::Address():
      family(FamilyUnknown),
      component(neutron::ComponentUnknown),
      digest(nullptr)
    {
    }

    ///
    /// this is the copy constructor.
    ///
    Address::Address(const Address&                             address):
      Object(address)
    {
      // set the family/component;
      this->family = address.family;
      this->component = address.component;

      // copy the digest, if present.
      if (address.digest != nullptr)
        {
          this->digest = new elle::cryptography::Digest(*address.digest);
        }
      else
        {
          this->digest = nullptr;
        }
    }

    ///
    /// this method reinitializes the object.
    ///
    Address::~Address()
    {
      // release the resources.
      if (this->digest != nullptr)
        delete this->digest;
    }

//
// ---------- methosd ---------------------------------------------------------
//

    elle::String const
    Address::unique() const
    {
      assert(this->digest != nullptr);

      // note that a unique element i.e the digest has already been computed
      // when the address was created.
      //
      // therefore, this method simply returns a string representation of
      // the digest.
      /* XXX[this does not work because Save() call the serialization which
             prepends a version number etc. leading to a non-uniform hexadecimal
             representation]
         elle::io::Unique unique;
         this->digest->Save(unique);
      */
      /* XXX[likewise for this one which consists in serializing in hexadecimal]
         std::stringstream ss;
         elle::serialize::HexadecimalArchive<elle::serialize::ArchiveMode::Output>
           archive(ss);
         archive << *this->digest;
      */
      elle::Natural32 i;
      elle::String alphabet("0123456789abcdef");
      elle::String string;

      for (i = 0; i < this->digest->region.size; i++)
        {
          elle::Character hexadecimal[2];

          hexadecimal[0] =
            alphabet[(this->digest->region.contents[i] >> 4) & 0xf];
          hexadecimal[1] =
            alphabet[this->digest->region.contents[i] & 0xf];

          string.append(hexadecimal, 2);
        }

      return (string);
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Address::operator==(const Address&      element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // if both are nullptr or equal return true, false otherwise
      if ((this->digest == nullptr) || (element.digest == nullptr))
        {
          if (this->digest != element.digest)
            return false;
        }
      else
        {
          if (*this->digest != *element.digest)
            return false;
        }

      return true;
    }

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Address::operator<(const Address&       element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return false;

      // test for a null digest.
      if ((this->digest == nullptr) && (element.digest == nullptr))
        return false;
      else if (this->digest == nullptr)
        return true;
      else if (element.digest == nullptr)
        return false;

      // compare the digests.
      if (*this->digest < *element.digest)
        return true;

      return false;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Address, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an address object.
    ///
    elle::Status        Address::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      // check the value.
      if (*this == Address::Null)
        {
          std::cout << alignment << "[Address] " << elle::none << std::endl;
        }
      else if (*this == Address::Some)
        {
          std::cout << alignment << "[Address] " << "(undef)" << std::endl;
        }
      else
        {
          // display the name.
          std::cout << alignment << "[Address]" << std::endl;

          // display the family.
          std::cout << alignment << elle::io::Dumpable::Shift << "[Family] "
                    << this->family << std::endl;

          // display the component.
          std::cout << alignment << elle::io::Dumpable::Shift << "[Component] "
                    << this->component << std::endl;

          // dump the digest.
          if (this->digest->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the digest");
        }

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Address::print(std::ostream& stream) const
    {
      stream << this->unique();
    }

  }
}
