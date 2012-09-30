#include <nucleus/neutron/Author.hh>

#include <elle/standalone/Log.hh>
#include <elle/standalone/Report.hh>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constants defines a null author.
    ///
    const Author                        Author::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    Author::Author():
      role(Object::RoleUnknown)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a owner-specific author object.
    ///
    elle::Status        Author::Create()
    {
      // set the role.
      this->role = Object::RoleOwner;

      return elle::Status::Ok;
    }

    ///
    /// this method creates a lord-specific author object, used whenver
    /// a user has been directly granted access to an object i.e is
    /// explicitely listed in the Access block.
    ///
    elle::Status        Author::Create(const Index&             index)
    {
      // set the role.
      this->role = Object::RoleLord;

      // set the index.
      this->lord.index = index;

      return elle::Status::Ok;
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Author::operator ==(Author const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      // compare the role.
      if (this->role != other.role)
        return false;

      // depending on the role.
      switch (this->role)
        {
        case Object::RoleOwner:
          {
            //
            // nothing more to compare.
            //

            break;
          }
        case Object::RoleLord:
          {
            //
            // compare the index to the entry in the Access block.
            //

            // compare the indexes.
            if (this->lord.index != other.lord.index)
              return false;

            break;
          }
        case Object::RoleVassal:
          {
            // XXX to implement.

            break;
          }
        default:
          {
            //
            // nothing more to compare.
            //

            break;
          }
        }

      return true;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an address object.
    ///
    elle::Status        Author::Dump(elle::Natural32            margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Author]" << std::endl;

      // dump the role.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Role] "
                << std::dec << this->role << std::endl;

      // depending on the role.
      switch (this->role)
        {
        case Object::RoleOwner:
          {
            //
            // nothing more to dump.
            //

            break;
          }
        case Object::RoleLord:
          {
            //
            // dump the index to the entry in the Access block.
            //

            // dump the index.
            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Index] " << std::dec << this->lord.index
                      << std::endl;

            break;
          }
        case Object::RoleVassal:
          {
            // XXX to implement.

            break;
          }
        case Object::RoleUnknown:
        case Object::RoleNone:
          {
            break;
          }
        default:
          {
            escape("unknown role '%u'", this->role);
          }
        }

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Author::print(std::ostream& stream) const
    {
      stream << "author("
             << this->role
             << ")";
    }

  }
}
