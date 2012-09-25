#include <nucleus/proton/Stamp.hh>

#include <elle/serialize/TupleSerializer.hxx>
#include <elle/cryptography/PrivateKey.hh>

#include <Infinit.hh>

namespace nucleus
{
  namespace proton
  {

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
    elle::Status        Stamp::Seal(elle::cryptography::PrivateKey const&     k)
    {
      // sign the attributes.
      if (k.Sign(elle::serialize::make_tuple(this->master, this->slave),
                 this->signature) == elle::Status::Error)
        escape("unable to sign the attributes");

      return elle::Status::Ok;
    }

    ///
    /// this method verifies that the signature has been issued by the
    /// oracle.
    ///
    elle::Status        Stamp::Validate()
    {
      // sign the attributes.
      if (Infinit::Authority.K.Verify(
            this->signature,
            elle::serialize::make_tuple(this->master, this->slave)) == elle::Status::Error)
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

      // compare the attributes.
      if ((this->master != other.master) ||
          (this->slave != other.slave) ||
          (this->signature != other.signature))
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
      if (this->signature.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the signature");

      return elle::Status::Ok;
    }

  }
}
