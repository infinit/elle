
#include <elle/serialize/TupleSerializer.hxx>
#include <nucleus/proton/Location.hh>

#include <nucleus/proton/Stamp.hh>

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
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Stamp::operator==(const Stamp&          element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the attributes.
      if ((this->master != element.master) ||
          (this->slave != element.slave) ||
          (this->signature != element.signature))
        return false;

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Stamp, _());

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

      std::cout << alignment << elle::Dumpable::Shift
                << "[Master]" << std::endl;

      if (this->master.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the master");

      // dump the master.

      std::cout << alignment << elle::Dumpable::Shift
                << "[Slave]" << std::endl;

      if (this->slave.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the slave");

      // dump the signature.
      if (this->signature.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the signature");

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the stamp.
    ///
    //elle::Status        Stamp::Serialize(elle::Archive&         archive) const
    //{
    //  // serialize the attributes.
    //  if (archive.Serialize(this->master,
    //                        this->slave,
    //                        this->signature) == elle::Status::Error)
    //    escape("unable to serialize the attributes");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the stamp.
    /////
    //elle::Status        Stamp::Extract(elle::Archive&           archive)
    //{
    //  // extract the attributes.
    //  if (archive.Extract(this->master,
    //                      this->slave,
    //                      this->signature) == elle::Status::Error)
    //    escape("unable to extract the attributes");

    //  return elle::Status::Ok;
    //}

  }
}
