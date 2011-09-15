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
    elle::Status	Reference::Bind(const elle::String&	target)
    {
      enter();

      // set the target.
      this->target = target;

      // set the reference as dirty.
      this->_state = proton::StateDirty;

      leave();
    }

    ///
    /// this method retrieve the target.
    ///
    elle::Status	Reference::Resolve(elle::String&	target) const
    {
      enter();

      // retrieve the target.
      target = this->target;

      leave();
    }

    ///
    /// this method returns the length of the referenced way.
    ///
    elle::Status	Reference::Capacity(Size&		size) const
    {
      enter();

      // retrieve the length.
      size = static_cast<Size>(this->target.length());

      leave();
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
    elle::Status	Reference::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Reference] " << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift << "[_State] "
		<< this->_state << std::endl;

      // dump the target.
      std::cout << alignment << elle::Dumpable::Shift << "[Target] "
		<< this->target << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    elle::Status	Reference::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the target.
      if (archive.Serialize(this->target) == elle::StatusError)
	escape("unable to serialize the target");

      leave();
    }

    ///
    /// this method extracts the object.
    ///
    elle::Status	Reference::Extract(elle::Archive&	archive)
    {
      enter();

      // extract the target.
      if (archive.Extract(this->target) == elle::StatusError)
	escape("unable to extract the target");

      leave();
    }

  }
}
