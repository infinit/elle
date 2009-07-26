//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Voucher.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [sat jul 25 02:03:43 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Voucher.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		Voucher::Class = "Voucher";

//
// ---------- methods ---------------------------------------------------------
//

    // XXX

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Status		Voucher::New(Voucher&			voucher)
    {
      leave();
    }

    ///
    /// this method reinitializes the object.
    ///
    Status		Voucher::Delete(Voucher&		voucher)
    {
      leave();
    }

    ///
    /// assign the publickey.
    ///
    Voucher&		Voucher::operator=(const Voucher&	element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // reinitialize the object.
      if ((Voucher::Delete(*this) == StatusError) ||
	  (Voucher::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      // copy the attributes.
      this->consumer = element.consumer;
      this->signature = element.signature;

      return (*this);
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Voucher::operator==(const Voucher&	element)
    {
      return ((this->consumer == element.consumer) &&
	      (this->signature == element.signature));
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Voucher::operator!=(const Voucher&	element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an voucher object.
    ///
    Status		Voucher::Dump(Natural32			margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Voucher]" << std::endl;

      if (this->consumer.Dump(margin + 2) == StatusError)
	escape("unable to dump the consumer");

      if (this->signature.Dump(margin + 2) == StatusError)
	escape("unable to dump the signature");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the voucher object.
    ///
    Status		Voucher::Serialize(Archive&		archive) const
    {
      Archive		ar;

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Voucher::Class) == StatusError)
	escape("unable to serialize the class name");

      // serialize the attributes.
      if (ar.Serialize(this->consumer) == StatusError)
	escape("unable to serialize the consumer");

      if (ar.Serialize(this->signature) == StatusError)
	escape("unable to serialize the signature");

      // record in the archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object's archive");

      leave();
    }

    ///
    /// this method extracts the voucher object.
    ///
    Status		Voucher::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;
      Archive::Type	type;

      // extract the voucher archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the voucher archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // check the name.
      if (Voucher::Class != name)
	escape("wrong class name in the extract object");

      // extract the attributes.
      if (ar.Extract(this->consumer) == StatusError)
	escape("unable to extract the consumer");

      if (ar.Extract(this->signature) == StatusError)
	escape("unable to extract the signature");

      leave();
    }

  }
}
