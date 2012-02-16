//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sun sep 25 21:04:11 2011]
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
      _footprint(*this)
    {
    }

    ///
    /// object-specific constructor.
    ///
    template <typename V,
              typename T>
    Inlet<V, T>::Inlet(const typename V::K&                     key,
                       T*                                       object):
      key(key),
      value(object),

      _footprint(*this)
    {
    }

    ///
    /// address-specific constructor.
    ///
    template <typename V,
              typename T>
    Inlet<V, T>::Inlet(const typename V::K&                     key,
                       const Address&                           address):
      key(key),
      value(address),

      _footprint(*this)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the inlet.
    ///
    template <typename V,
              typename T>
    elle::Status        Inlet<V, T>::Dump(const elle::Natural32 margin)
      const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Inlet] " << this << std::endl;

      // dump the key.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Key] " << std::dec << this->key << std::endl;
      // XXX remove std::dec

      // dump the value.
      if (this->value.Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the value");

      // dump the footprint.
      std::cout << alignment << elle::Dumpable::Shift
                << "[_Footprint]" << std::endl;

      if (this->_footprint.Dump(margin + 4) == elle::StatusError)
        escape("unable to dump the footprint");

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the inlet attributes.
    ///
    template <typename V,
              typename T>
    elle::Status        Inlet<V, T>::Serialize(elle::Archive&   archive) const
    {
      // serialize the attributes.
      if (archive.Serialize(this->key,
                            this->value) == elle::StatusError)
        escape("unable to serialize the attributes");

      return elle::StatusOk;
    }

    ///
    /// this method extracts the attributes.
    ///
    template <typename V,
              typename T>
    elle::Status        Inlet<V, T>::Extract(elle::Archive&     archive)
    {
      // extracts the attributes.
      if (archive.Extract(this->key,
                          this->value) == elle::StatusError)
        escape("unable to extract the attributes");

      return elle::StatusOk;
    }

  }
}

#endif
