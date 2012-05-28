//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon aug 17 02:05:03 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Subject.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this table maintains a mapping between subject types and their
    /// respective human-readable representations.
    ///
    const Subject::Descriptor   Subject::Descriptors[Subject::Types] =
      {
        { Subject::TypeUnknown, "unknown" },
        { Subject::TypeUser, "user" },
        { Subject::TypeGroup, "group" },
      };

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns the type associated with the given string.
    ///
    elle::Status        Subject::Convert(const elle::String&    name,
                                         Type&                  type)
    {
      elle::String      string(name);
      elle::Natural32   i;

      // transform the given name in lowercase.
      std::transform(string.begin(), string.end(),
                     string.begin(), std::ptr_fun(::tolower));

      // go through the descriptors.
      for (i = 0; i < Subject::Types; i++)
        {
          // is this the type we are looking for?
          if (Subject::Descriptors[i].name == string)
            {
              // set the type.
              type = Subject::Descriptors[i].type;

              return elle::StatusOk;
            }
        }

      escape("unable to locate the given entity name");
    }

    ///
    /// this method converts a type into its human-readable representation.
    ///
    elle::Status        Subject::Convert(const Type             type,
                                         elle::String&          name)
    {
      elle::Natural32   i;

      // go through the descriptors.
      for (i = 0; i < Subject::Types; i++)
        {
          // is this the entity we are looking for?
          if (Subject::Descriptors[i].type == type)
            {
              // set the name.
              name = Subject::Descriptors[i].name;

              return elle::StatusOk;
            }
        }

      escape("unable to locate the given type");
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    Subject::Subject():
      type(Subject::TypeUnknown),
      user(NULL)
    {
    }

    ///
    /// copy constructor.
    ///
    Subject::Subject(const Subject&                             subject):
      Object(subject)
    {
      // set the type.
      this->type = subject.type;

      switch (this->type)
        {
        case Subject::TypeUser:
          {
            // copy the user public key.
            this->user = new elle::PublicKey(*subject.user);

            break;
          }
        case Subject::TypeGroup:
          {
            // copy the group address.
            this->group = new proton::Address(*subject.group);

            break;
          }
        default:
          {
            break;
          }
        }
    }

    ///
    /// the destructor.
    ///
    Subject::~Subject()
    {
      switch (this->type)
        {
        case Subject::TypeUser:
          {
            delete this->user;

            break;
          }
        case Subject::TypeGroup:
          {
            delete this->group;

            break;
          }
        default:
          {
            break;
          }
        }
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a user subject.
    ///
    elle::Status        Subject::Create(const elle::PublicKey&  K)
    {
      // set the type.
      this->type = Subject::TypeUser;

      // allocate and copy the key.
      this->user = new elle::PublicKey(K);

      return elle::StatusOk;
    }

    ///
    /// this method creates a group subject.
    ///
    elle::Status        Subject::Create(const proton::Address&  descriptor)
    {
      // set the type.
      this->type = Subject::TypeGroup;

      // allocate the address.
      this->group = new proton::Address(descriptor);

      return elle::StatusOk;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Subject::operator==(const Subject&      element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::StatusTrue;

      // compare the type.
      if (this->type != element.type)
        return elle::StatusFalse;

      // compare the identifier.
      switch (this->type)
        {
        case Subject::TypeUser:
          {
            return (*this->user == *element.user);
          }
        case Subject::TypeGroup:
          {
            return (*this->group == *element.group);
          }
        default:
          {
            break;
          }
        }

      return elle::StatusTrue;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Subject, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an time object.
    ///
    elle::Status        Subject::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Subject]" << std::endl;

      // display the type.
      std::cout << alignment << elle::Dumpable::Shift << "[Type] "
                << this->type << std::endl;

      // compare the identifier.
      switch (this->type)
        {
        case Subject::TypeUser:
          {
            std::cout << alignment << elle::Dumpable::Shift
                      << "[Identifier]" << std::endl;

            // dump the user public key.
            if (this->user->Dump(margin + 4) == elle::StatusError)
              escape("unable to dump the user's public key");

            break;
          }
        case Subject::TypeGroup:
          {
            std::cout << alignment << elle::Dumpable::Shift
                      << "[Identifier]" << std::endl;

            // dump the group address.
            if (this->group->Dump(margin + 4) == elle::StatusError)
              escape("unable to dump the group address");

            break;
          }
        default:
          {
            break;
          }
        }

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the subject.
    ///
    elle::Status        Subject::Serialize(elle::Archive&       archive) const
    {
      // serialize the type.
      if (archive.Serialize(static_cast<elle::Natural8>(this->type)) ==
          elle::StatusError)
        escape("unable to serialize the type");

      // serialize the identifier.
      switch (this->type)
        {
        case Subject::TypeUser:
          {
            // serialize the user public key.
            if (archive.Serialize(*this->user) == elle::StatusError)
              escape("unable to serialize the user public key");

            break;
          }
        case Subject::TypeGroup:
          {
            // serialize the group address.
            if (archive.Serialize(*this->group) == elle::StatusError)
              escape("unable to serialize the group address");

            break;
          }
        default:
          {
            break;
          }
        }

      return elle::StatusOk;
    }

    ///
    /// this method extracts the subject.
    ///
    elle::Status        Subject::Extract(elle::Archive&         archive)
    {
      // extract the type.
      if (archive.Extract(reinterpret_cast<elle::Natural8&>(this->type)) ==
          elle::StatusError)
        escape("unable to extract the type");

      // extract the identifier.
      switch (this->type)
        {
        case Subject::TypeUser:
          {
            // allocate a new public key.
            this->user = new elle::PublicKey;

            // extract the user public key.
            if (archive.Extract(*this->user) == elle::StatusError)
              escape("unable to extract the user public key");

            break;
          }
        case Subject::TypeGroup:
          {
            // allocate a new address.
            this->group = new proton::Address;

            // extract the group address.
            if (archive.Extract(*this->group) == elle::StatusError)
              escape("unable to extract the group address");

            break;
          }
        default:
          {
            break;
          }
        }

      return elle::StatusOk;
    }

  }
}
