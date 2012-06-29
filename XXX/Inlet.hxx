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
      footprint(*this)
    {
    }

    ///
    /// object-specific constructor.
    ///
    template <typename V,
              typename T>
    Inlet<V, T>::Inlet(const typename V::K&                     key,
                       Handle&                                  value):
      key(key),
      value(value),
      footprint(*this)
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
                << "[Key] " << this->key << std::endl;

      // dump the value.
      if (this->value.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the value");

      // dump the footprint.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Footprint]" << std::endl;

      if (this->footprint.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the footprint");

      return elle::Status::Ok;
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
                            this->value) == elle::Status::Error)
        escape("unable to serialize the attributes");

      return elle::Status::Ok;
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
                          this->value) == elle::Status::Error)
        escape("unable to extract the attributes");

      return elle::Status::Ok;
    }

  }
}

#endif
#ifndef  XXX_INLET_HXX
# define XXX_INLET_HXX

# include <cassert>

# include <XXX/Handle.hh>

# include <XXX/Inlet.hh>

ELLE_SERIALIZE_SIMPLE_T2(XXX::Inlet,
                         archive,
                         value,
                         version)
{
  assert(version == 0);

  archive & value.key;
  archive & value.value;
}

#endif
