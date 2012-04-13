//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [tue feb 17 12:39:45 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Reference.hh>

#include <nucleus/proton/Contents.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Reference::Reference(proton::Contents<Reference>&           contents):
      contents(contents)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the data.
    ///
    elle::Status        Reference::Create()
    {
      this->contents.state = proton::StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method sets the target way.
    ///
    elle::Status        Reference::Bind(const elle::String&     target)
    {
      // set the target.
      this->target = target;

      // set the reference as dirty.
      this->contents.state = proton::StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method retrieve the target.
    ///
    elle::Status        Reference::Resolve(elle::String&        target) const
    {
      // retrieve the target.
      target = this->target;

      return elle::Status::Ok;
    }

    ///
    /// this method returns the length of the referenced way.
    ///
    elle::Status        Reference::Capacity(Size&               size) const
    {
      // retrieve the length.
      size = static_cast<Size>(this->target.length());

      return elle::Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the object.
    ///
    elle::Status        Reference::Dump(elle::Natural32         margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Reference] " << std::endl;

      // dump the target.
      std::cout << alignment << elle::Dumpable::Shift << "[Target] "
                << this->target << std::endl;

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    elle::Status        Reference::Serialize(elle::Archive&     archive) const
    {
      // serialize the target.
      if (archive.Serialize(this->target) == elle::Status::Error)
        escape("unable to serialize the target");

      return elle::Status::Ok;
    }

    ///
    /// this method extracts the object.
    ///
    elle::Status        Reference::Extract(elle::Archive&       archive)
    {
      // extract the target.
      if (archive.Extract(this->target) == elle::Status::Error)
        escape("unable to extract the target");

      return elle::Status::Ok;
    }

  }
}
