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

    ///
    /// the constructor.
    ///
    Author::Author():
      role(RoleUnknown)
    {
    }

    ///
    /// the destructor.
    ///
    Author::~Author()
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
      this->role = RoleOwner;

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
      this->role = RoleLord;

      // set the index.
      this->lord.index = index;

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Author::operator==(const Author&        element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the role.
      if (this->role != element.role)
        return false;

      // depending on the role.
      switch (this->role)
        {
        case RoleOwner:
          {
            //
            // nothing more to compare.
            //

            break;
          }
        case RoleLord:
          {
            //
            // compare the index to the entry in the Access block.
            //

            // compare the indexes.
            if (this->lord.index != element.lord.index)
              return false;

            break;
          }
        case RoleVassal:
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

    ///
    /// this macro-function call generates the object.
    ///
    embed(Author, _());

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
        case RoleOwner:
          {
            //
            // nothing more to dump.
            //

            break;
          }
        case RoleLord:
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
        case RoleVassal:
          {
            // XXX to implement.

            break;
          }
        }

      return elle::Status::Ok;
    }

  }
}
