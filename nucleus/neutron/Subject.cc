#include <nucleus/neutron/Subject.hh>
#include <nucleus/Exception.hh>

#include <algorithm>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definitions -----------------------------------------------------
//

    const Subject::Descriptor Subject::_descriptors[Subject::Types] =
      {
        { Subject::TypeUnknown, "unknown" },
        { Subject::TypeUser, "user" },
        { Subject::TypeGroup, "group" },
      };

//
// ---------- static methods --------------------------------------------------
//

    elle::Status
    Subject::Convert(elle::String const& name,
                     Type& type)
    {
      elle::String string(name);
      elle::Natural32 i;

      // Transform the given name in lowercase.
      std::transform(string.begin(), string.end(),
                     string.begin(), std::ptr_fun(::tolower));

      for (i = 0; i < Subject::Types; i++)
        {
          // Is this the name we are looking for?
          if (Subject::_descriptors[i].name == string)
            {
              // Set the type.
              type = Subject::_descriptors[i].type;

              return elle::Status::Ok;
            }
        }

      throw Exception("unable to locate the given entity name");
    }

    elle::Status
    Subject::Convert(Type const type,
                     elle::String& name)
    {
      elle::Natural32   i;

      for (i = 0; i < Subject::Types; i++)
        {
          // Is this the type we are looking for?
          if (Subject::_descriptors[i].type == type)
            {
              // Set the name.
              name = Subject::_descriptors[i].name;

              return elle::Status::Ok;
            }
        }

      throw Exception("unable to locate the given type");
    }

//
// ---------- construction ----------------------------------------------------
//

    Subject::Subject():
      _type(Subject::TypeUnknown),
      _user(nullptr)
    {
      // Manually set all the union pointers to null so as to make all
      // the cases are handled.
      this->_user = nullptr;
      this->_group = nullptr;
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

    Subject::Subject(Subject const& other):
      _type(other._type)
    {
      // Manually set all the union pointers to null so as to make sure all
      // the cases are handled.
      this->_user = nullptr;
      this->_group = nullptr;

      switch (other._type)
        {
        case Subject::TypeUser:
          {
            // Copy the user public key.
            this->_user = new typename User::Identity(*other._user);

            break;
          }
        case Subject::TypeGroup:
          {
            // Copy the group address.
            this->_group = new typename Group::Identity(*other._group);

            break;
          }
        default:
          {
            break;
          }
        }
    }

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

    elle::Status
    Subject::Create(typename User::Identity const& identity)
    {
      // set the type.
      this->_type = Subject::TypeUser;

      // allocate and copy the key.
      this->_user = new typename User::Identity(identity);

      return elle::Status::Ok;
    }

    elle::Status
    Subject::Create(typename Group::Identity const& identity)
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
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Subject::operator ==(Subject const& other) const
    {
      if (this == &other)
        return (true);

      // Compare the type.
      if (this->_type != other._type)
        return (false);

      // Compare the identifier.
      switch (this->_type)
        {
        case Subject::TypeUser:
          {
            return (*this->_user == *other._user);
          }
        case Subject::TypeGroup:
          {
            return (*this->_group == *other._group);
          }
        default:
          {
            // XXX
            ELLE_ASSERT(false);
            break;
          }
        }

      elle::unreachable();
    }

    elle::Boolean
    Subject::operator <(Subject const& other) const
    {
      if (this == &other)
        return (false);

      // Compare the type.
      if (this->_type < other._type)
        return (true);
      else if (this->_type > other._type)
        return (false);

      // Compare the identifier.
      switch (this->_type)
        {
        case Subject::TypeUser:
          {
            return (*this->_user < *other._user);
          }
        case Subject::TypeGroup:
          {
            return (*this->_group < *other._group);
          }
        default:
          {
            // XXX
            ELLE_ASSERT(false);
            break;
          }
        }

      elle::unreachable();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status        Subject::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Subject]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[Type] "
                << this->_type << std::endl;

      switch (this->_type)
        {
        case Subject::TypeUser:
          {
            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Identifier] " << *this->_user << std::endl;

            break;
          }
        case Subject::TypeGroup:
          {
            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Identifier] " << *this->_group << std::endl;

            break;
          }
        default:
          {
            break;
          }
        }

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Subject::print(std::ostream& stream) const
    {
      switch (this->_type)
        {
        case TypeUnknown:
          {
            stream << "subject(unknown)";
            break;
          }
        case TypeUser:
          {
            stream << "user("
                   << *this->_user
                   << ")";
            break;
          }
        case TypeGroup:
          {
            stream << "group("
                   << *this->_group
                   << ")";
            break;
          }
        default:
          {
            throw Exception("unknown subject '%s'", this->_type);
          }
        }
    }

  }
}
