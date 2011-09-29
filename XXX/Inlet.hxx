//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien.quintard   [sun sep 25 21:04:11 2011]
//

#ifndef NUCLEUS_PROTON_INLET_HXX
#define NUCLEUS_PROTON_INLET_HXX

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V,
	      typename T>
    Inlet<V, T>::Inlet():
      address(Address::Vacuum),
      _value(NULL),
      _footprint(*this)
    {
    }

    ///
    /// XXX
    ///
    template <typename V,
	      typename T>
    Inlet<V, T>::Inlet(const typename V::K&			key,
		       T*					value):
      key(key),
      address(Address::Vacuum),
      _value(value),
      _footprint(*this)
    {
    }

    ///
    /// XXX
    ///
    template <typename V,
	      typename T>
    Inlet<V, T>::Inlet(const typename V::K&			key,
		       const Address&				address):
      key(key),
      address(address),
      _value(NULL),
      _footprint(*this)
    {
    }

    ///
    /// XXX
    ///
    template <typename V,
	      typename T>
    Inlet<V, T>::~Inlet()
    {
      // delete the value, if present.
      if (this->_value != NULL)
	delete this->_value;
    }

    ///
    /// XXX
    ///
    template <typename V,
	      typename T>
    elle::Status	Inlet<V, T>::Dump(const elle::Natural32	margin)
      const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Inlet] " << this << std::endl;

      // dump the key.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Key] " << std::dec << this->key << std::endl;
      // XXX remove std::dec

      // dump the address.
      if (this->address.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the address");

      // dump the value, if present.
      if (this->_value != NULL)
	{
	  if (this->_value->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the value");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << elle::none << std::endl;
	}

      // dump the footprint.
      if (this->_footprint.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the footprint");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the inlet attributes.
    ///
    template <typename V,
	      typename T>
    elle::Status	Inlet<V, T>::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->key,
			    this->address) == elle::StatusError)
	escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the attributes.
    ///
    template <typename V,
	      typename T>
    elle::Status	Inlet<V, T>::Extract(elle::Archive&	archive)
    {
      enter();

      // extracts the attributes.
      if (archive.Extract(this->key,
			  this->address) == elle::StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}

#endif
