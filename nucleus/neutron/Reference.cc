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
    Reference::Reference():
      _state(proton::StateClean)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets the target way.
    ///
    elle::Status        Reference::Bind(const elle::String&     target)
    {
      // set the target.
      this->target = target;

      // set the reference as dirty.
      this->_state = proton::StateDirty;

      return elle::StatusOk;
    }

    ///
    /// this method retrieve the target.
    ///
    elle::Status        Reference::Resolve(elle::String&        target) const
    {
      // retrieve the target.
      target = this->target;

      return elle::StatusOk;
    }

    ///
    /// this method returns the length of the referenced way.
    ///
    elle::Status        Reference::Capacity(Size&               size) const
    {
      // retrieve the length.
      size = static_cast<Size>(this->target.length());

      return elle::StatusOk;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Reference, _());

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

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift << "[_State] "
                << this->_state << std::endl;

      // dump the target.
      std::cout << alignment << elle::Dumpable::Shift << "[Target] "
                << this->target << std::endl;

      return elle::StatusOk;
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
      if (archive.Serialize(this->target) == elle::StatusError)
        escape("unable to serialize the target");

      return elle::StatusOk;
    }

    ///
    /// this method extracts the object.
    ///
    elle::Status        Reference::Extract(elle::Archive&       archive)
    {
      // extract the target.
      if (archive.Extract(this->target) == elle::StatusError)
        escape("unable to extract the target");

      return elle::StatusOk;
    }

  }
}
