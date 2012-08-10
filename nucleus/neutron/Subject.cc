#include <nucleus/neutron/Subject.hh>
#include <nucleus/Exception.hh>

#include <elle/idiom/Close.hh>
# include <algorithm>
#include <elle/idiom/Open.hh>

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

              return elle::Status::Ok;
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

              return elle::Status::Ok;
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
      _type(Subject::TypeUnknown),
      _user(nullptr)
    {
    }

    Subject::Subject(typename User::Identity const& identity):
      _type(Subject::TypeUser),
      _user(new typename User::Identity(identity))
    {
    }

    Subject::Subject(typename Group::Identity const& identity):
      _type(Subject::TypeGroup),
      _group(new typename Group::Identity(identity))
    {
    }

    ///
    /// copy constructor.
    ///
    Subject::Subject(Subject const& other):
      Object(other)
    {
      // set the type.
      this->_type = other.type();

      switch (this->_type)
        {
        case Subject::TypeUser:
          {
            // copy the user public key.
            this->_user = new typename User::Identity(other.user());

            break;
          }
        case Subject::TypeGroup:
          {
            // copy the group address.
            this->_group = new typename Group::Identity(other.group());

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
      switch (this->_type)
        {
        case Subject::TypeUser:
          {
            delete this->_user;

            break;
          }
        case Subject::TypeGroup:
          {
            delete this->_group;

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
    elle::Status        Subject::Create(typename User::Identity const& identity)
    {
      // set the type.
      this->_type = Subject::TypeUser;

      // allocate and copy the key.
      this->_user = new typename User::Identity(identity);

      return elle::Status::Ok;
    }

    ///
    /// this method creates a group subject.
    ///
    elle::Status        Subject::Create(typename Group::Identity const& identity)
    {
      // set the type.
      this->_type = Subject::TypeGroup;

      // allocate the address.
      this->_group = new typename Group::Identity(identity);

      return elle::Status::Ok;
    }

    Subject::Type
    Subject::type() const
    {
      return (this->_type);
    }

    typename User::Identity const&
    Subject::user() const
    {
      if (this->_type != Subject::TypeUser)
        throw Exception("this subject does not represent a user");

      return (*this->_user);
    }

    typename Group::Identity const&
    Subject::group() const
    {
      if (this->_type != Subject::TypeGroup)
        throw Exception("this subject does not represent a group");

      return (*this->_group);
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
        return true;

      // compare the type.
      if (this->_type != element.type())
        return false;

      // compare the identifier.
      switch (this->_type)
        {
        case Subject::TypeUser:
          {
            return (*this->_user == element.user());
          }
        case Subject::TypeGroup:
          {
            return (*this->_group == element.group());
          }
        default:
          {
            break;
          }
        }

      return true;
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
      std::cout << alignment << elle::io::Dumpable::Shift << "[Type] "
                << this->_type << std::endl;

      // compare the identifier.
      switch (this->_type)
        {
        case Subject::TypeUser:
          {
            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Identifier]" << std::endl;

            // dump the user public key.
            if (this->_user->Dump(margin + 4) == elle::Status::Error)
              escape("unable to dump the user's public key");

            break;
          }
        case Subject::TypeGroup:
          {
            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Identifier]" << std::endl;

            // dump the group address.
            if (this->_group->Dump(margin + 4) == elle::Status::Error)
              escape("unable to dump the group address");

            break;
          }
        default:
          {
            break;
          }
        }

      return elle::Status::Ok;
    }

  }
}
