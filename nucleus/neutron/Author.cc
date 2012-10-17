#include <nucleus/neutron/Author.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {
    const Author Author::Null(Object::RoleUnknown);

    /*-------------.
    | Construction |
    `-------------*/

    Author::Author():
      role(Object::RoleOwner)
    {}

    Author::Author(Index const& index):
      role(Object::RoleLord)
    {
      this->lord.index = index;
    }

    Author::Author(Object::Role role):
      role(Object::RoleUnknown)
    {}

    /*----------.
    | Operators |
    `----------*/

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
