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
// updated       julien quintard   [tue dec  1 03:20:06 2009]
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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    Reference::Reference():
      Contents(hole::Block::KindReference)
    {
    }

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
