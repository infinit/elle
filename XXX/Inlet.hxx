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
    /// default constructor.
    ///
    template <typename V,
	      typename T>
    Inlet<V, T>::Inlet():
      value(Address::Some),

      _footprint(*this),
      _value(NULL)
    {
    }

    ///
    /// object-specific constructor.
    ///
    template <typename V,
	      typename T>
    Inlet<V, T>::Inlet(const typename V::K&			key,
		       T*					value):
      key(key),
      value(Address::Some),

      _footprint(*this),
      _value(value)
    {
    }

    ///
    /// address-specific constructor.
    ///
    template <typename V,
	      typename T>
    Inlet<V, T>::Inlet(const typename V::K&			key,
		       const Address&				value):
      key(key),
      value(value),

      _footprint(*this),
      _value(NULL)
    {
    }

    ///
    /// destructor.
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
    /// this method dumps the inlet.
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

      /* XXX
      // dump the value.
      if (this->value.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the value");
      */

      // dump the footprint.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[_Footprint]" << std::endl;

      if (this->_footprint.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the footprint");

      // dump the value, if present.
      if (this->_value != NULL)
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[_Value]" << std::endl;

	  if (this->_value->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the value");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[_Value] " << elle::none << std::endl;
	}

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
			    this->value) == elle::StatusError)
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
			  this->value) == elle::StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}

#endif
