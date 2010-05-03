//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Reference.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [mon may  3 23:04:17 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Reference.hh>

#include <etoile/path/Length.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Reference::Reference():
      state(StateClean)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets the target way.
    ///
    elle::Status	Reference::Bind(const path::Way&	target)
    {
      enter();

      // set the target.
      this->target = target;

      // set the reference as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method retrieve the target.
    ///
    elle::Status	Reference::Resolve(path::Way&		target) const
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
      path::Length	length;

      enter();

      // retrieve the length.
      if (this->target.Capacity(length) == elle::StatusError)
	escape("unable to retrieve the way length");

      // set the size.
      size = (Size)length;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Reference, _(), _());

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
      std::cout << alignment << elle::Dumpable::Shift << "[State] "
		<< this->state << std::endl;

      // dump the target.
      if (this->target.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the target");

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
