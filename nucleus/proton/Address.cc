//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon feb 16 21:42:37 2009]
//

//
// ---------- includes --------------------------------------------------------
//

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
      ;

      // create the any address with default meaningless values.
      if (Address::Any.Create(
            Address::Any.family, Address::Any.component,
            static_cast<elle::Natural8>(Address::Any.family),
            static_cast<elle::Natural8>(Address::Any.component)) ==
          elle::StatusError)
        escape("unable to create the any address");

      return elle::StatusOk;
    }

    ///
    /// this method cleans the address system.
    ///
    elle::Status        Address::Clean()
    {
      ;

      // nothing to do.

      return elle::StatusOk;
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
      digest(NULL)
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
      if (address.digest != NULL)
        {
          this->digest = new elle::Digest(*address.digest);
        }
      else
        {
          this->digest = NULL;
        }
    }

    ///
    /// this method reinitializes the object.
    ///
    Address::~Address()
    {
      // release the resources.
      if (this->digest != NULL)
        delete this->digest;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Address::operator==(const Address&      element) const
    {
      ;

      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::StatusTrue;

      // if both are NULL or equal return true, false otherwise
      if ((this->digest == NULL) || (element.digest == NULL))
        {
          if (this->digest != element.digest)
            return elle::StatusFalse;
        }
      else
        {
          if (*this->digest != *element.digest)
            return elle::StatusFalse;
        }

      return elle::StatusTrue;
    }

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Address::operator<(const Address&       element) const
    {
      ;

      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::StatusFalse;

      // test for a null digest.
      if ((this->digest == NULL) && (element.digest == NULL))
        return elle::StatusFalse;
      else if (this->digest == NULL)
        return elle::StatusTrue;
      else if (element.digest == NULL)
        return elle::StatusFalse;

      // compare the digests.
      if (*this->digest < *element.digest)
        return elle::StatusTrue;

      return elle::StatusFalse;
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

      ;

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
          std::cout << alignment << elle::Dumpable::Shift << "[Family] "
                    << this->family << std::endl;

          // display the component.
          std::cout << alignment << elle::Dumpable::Shift << "[Component] "
                    << this->component << std::endl;

          // display the address depending on its value.
          if (*this == Address::Null)
            {
              std::cout << alignment << elle::Dumpable::Shift
                        << "[Digest] " << elle::none << std::endl;
            }
          else
            {
              // dump the digest.
              if (this->digest->Dump(margin + 2) == elle::StatusError)
                escape("unable to dump the digest");
            }
        }

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the address object.
    ///
    elle::Status        Address::Serialize(elle::Archive&       archive) const
    {
      ;

      if (this->digest != NULL)
        {
          // serialize the internal digest.
          if (archive.Serialize(static_cast<elle::Natural8>(this->family),
                                static_cast<elle::Natural8>(this->component),
                                *this->digest) == elle::StatusError)
            escape("unable to serialize the digest");
        }
      else
        {
          // serialize 'none'.
          if (archive.Serialize(elle::none) == elle::StatusError)
            escape("unable to serialize 'none'");
        }

      return elle::StatusOk;
    }

    ///
    /// this method extracts the address object.
    ///
    elle::Status        Address::Extract(elle::Archive&         archive)
    {
      elle::Archive::Type       type;

      ;

      // fetch the next element's type.
      if (archive.Fetch(type) == elle::StatusError)
        escape("unable to fetch the next element's type");

      if (type == elle::Archive::TypeNull)
        {
          // nothing to do, keep the digest to NULL.
          if (archive.Extract(elle::none) == elle::StatusError)
            escape("unable to extract null");
        }
      else
        {
          // allocate a digest.
          this->digest = new elle::Digest;

          // extract the internal digest.
          if (archive.Extract(
                reinterpret_cast<elle::Natural8&>(this->family),
                reinterpret_cast<elle::Natural8&>(this->component),
                *this->digest) == elle::StatusError)
            escape("unable to extract the digest");
        }

      return elle::StatusOk;
    }

  }
}
