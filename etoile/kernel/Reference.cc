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
// updated       julien quintard   [tue apr 20 10:29:49 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Reference.hh>

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
    Status		Reference::Bind(const path::Way&	target)
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
    Status		Reference::Resolve(path::Way&		target) const
    {
      enter();

      // retrieve the target.
      target = this->target;

      leave();
    }

    ///
    /// this method returns the length of the referenced way.
    ///
    Status		Reference::Capacity(Size&		size) const
    {
      path::Length	length;

      enter();

      // retrieve the length.
      if (this->target.Capacity(length) == StatusError)
	escape("unable to retrieve the way length");

      // set the size.
      size = (Size)length;

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Reference);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the object.
    ///
    Status		Reference::Dump(Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Reference] " << std::endl;

      // dump the state.
      std::cout << alignment << Dumpable::Shift << "[State] "
		<< this->state << std::endl;

      // dump the target.
      if (this->target.Dump(margin + 2) == StatusError)
	escape("unable to dump the target");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    Status		Reference::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the target.
      if (archive.Serialize(this->target) == StatusError)
	escape("unable to serialize the target");

      leave();
    }

    ///
    /// this method extracts the object.
    ///
    Status		Reference::Extract(Archive&		archive)
    {
      enter();

      // extract the target.
      if (archive.Extract(this->target) == StatusError)
	escape("unable to extract the target");

      leave();
    }

  }
}
