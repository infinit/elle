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
// updated       julien quintard   [tue aug 11 00:44:28 2009]
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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// initialises a contents object.
    ///
    Status		Contents::Create(const Contents::Type	type)
    {
      // set the type.
      this->type = type;

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the contents.
    ///
    Contents&		Contents::operator=(const Contents&	element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // recycle the address.
      if (this->Recycle<Contents>(&element) == StatusError)
	yield("unable to recycle the contents", *this);

      return (*this);
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Contents::operator==(const Contents&	element) const
    {
      return ((this->type == element.type) &&
	      (this->address == element.address) &&
	      (this->size == element.size));
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Contents::operator!=(const Contents&	element) const
    {
      return (!(*this == element));
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

      std::cout << alignment << shift << "[Type] "
		<< this->type << std::endl;

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
      if (archive.Serialize((Byte&)this->type,
			    this->address,
			    this->size) == StatusError)
	escape("unable to serialize the contents' attributes");

      leave();
    }

    ///
    /// this method extracts the address object.
    ///
    Status		Contents::Extract(Archive&		archive)
    {
      // extract the attributes.
      if (archive.Extract((Byte&)this->type,
			  this->address,
			  this->size) == StatusError)
	escape("unable to extract the contents' attributes");

      leave();
    }

  }
}
