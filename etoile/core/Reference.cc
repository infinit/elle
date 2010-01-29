//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Reference.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [thu jan 28 22:08:04 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Reference.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the component identifier.
    ///
    const String		Reference::Identifier = "Reference";

//
// ---------- methods ---------------------------------------------------------
//

    // XXX

    ///
    /// XXX
    ///
    Status		Reference::Size(Natural64&		size) const
    {
      // set the size.
      size = this->target.length();

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		Reference::Dump(Natural32		margin) const
    {
      // XXX

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    Status		Reference::Serialize(Archive&		archive) const
    {
      // serialize the component name.
      if (archive.Serialize(Reference::Identifier) == StatusError)
	escape("unable to serialize the component identifier");

      // XXX

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		Reference::Extract(Archive&		archive)
    {
      // XXX

      leave();
    }

  }
}
