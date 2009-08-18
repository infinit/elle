//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Contents.cc
//
// created       julien quintard   [mon aug 10 23:48:12 2009]
// updated       julien quintard   [mon aug 17 02:05:49 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Contents.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor.
    ///
    Contents::Contents():
      size(0)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an address object.
    ///
    Status		Contents::Dump(Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Contents]" << std::endl;

      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the address");

      std::cout << alignment << shift << "[Size] "
		<< this->size << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the address object.
    ///
    Status		Contents::Serialize(Archive&		archive) const
    {
      // serialize the attributes.
      if (archive.Serialize(this->address, this->size) == StatusError)
	escape("unable to serialize the contents' attributes");

      leave();
    }

    ///
    /// this method extracts the address object.
    ///
    Status		Contents::Extract(Archive&		archive)
    {
      // extract the attributes.
      if (archive.Extract(this->address, this->size) == StatusError)
	escape("unable to extract the contents' attributes");

      leave();
    }

  }
}
