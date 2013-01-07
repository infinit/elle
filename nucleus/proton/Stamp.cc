#include <nucleus/proton/Stamp.hh>

#include <elle/serialize/TupleSerializer.hxx>
#include <cryptography/PrivateKey.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    Stamp::Stamp():
      _signature(nullptr)
    {
    }

    Stamp::~Stamp()
    {
      delete this->_signature;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method specifies the master/slave locations to stamp.
    ///
    elle::Status        Stamp::Create(const Location&           master,
                                      const Location&           slave)
    {
      // set the attributes.
      this->master = master;
      this->slave = slave;

      return elle::Status::Ok;
    }

    ///
    /// this method seals the stamp with the oracle's private key.
    ///
    elle::Status        Stamp::Seal(cryptography::PrivateKey const&     k)
    {
      // sign the attributes.
      delete this->_signature;
      this->_signature = nullptr;
      this->_signature =
        new cryptography::Signature{
          k.sign(elle::serialize::make_tuple(this->master, this->slave))};

      return elle::Status::Ok;
    }

    ///
    /// this method verifies that the signature has been issued by the
    /// oracle.
    ///
    elle::Status
    Stamp::Validate(elle::Authority const& authority)
    {
      ELLE_ASSERT(this->_signature != nullptr);

      // sign the attributes.
      if (authority.K().verify(
            *this->_signature,
            elle::serialize::make_tuple(this->master,
                                        this->slave)) == false)
        escape("this stamp seems not to have been issued by the oracle");

      return elle::Status::Ok;
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Stamp::operator ==(Stamp const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      ELLE_ASSERT(this->_signature != nullptr);
      ELLE_ASSERT(other._signature != nullptr);

      // compare the attributes.
      if ((this->master != other.master) ||
          (this->slave != other.slave) ||
          (*this->_signature != *other._signature))
        return false;

      return true;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a stamp.
    ///
    elle::Status        Stamp::Dump(elle::Natural32             margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Stamp]" << std::endl;

      // dump the master.

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Master]" << std::endl;

      if (this->master.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the master");

      // dump the master.

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Slave]" << std::endl;

      if (this->slave.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the slave");

      // dump the signature.
      if (this->_signature != nullptr)
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Signature] " << *this->_signature << std::endl;
        }

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Stamp::print(std::ostream& stream) const
    {
      stream << "stamp{"
             << this->master
             << ", "
             << this->slave
             << "}";
    }

  }
}
