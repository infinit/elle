//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri jul  8 10:44:04 2011]
//

#ifndef NUCLEUS_DERIVABLE_HXX
#define NUCLEUS_DERIVABLE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Derivable.hh>

namespace nucleus
{

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  template <typename T>
  Derivable<T>::Derivable():
    elle::Derivable<T>(Nucleus::Factory)
  {
  }

  ///
  /// specific constructor specifying the object; used whenever such
  /// an instance is about to be serialized.
  ///
  template <typename T>
  Derivable<T>::Derivable(const elle::Product&			product,
			  const T&				object):
    elle::Derivable<T>(product, object)
  {
  }

  ///
  /// specific constructor specifying the object. this construct is
  /// useful in order to extract the data directly into the given object
  /// without allocating a new object.
  ///
  template <typename T>
  Derivable<T>::Derivable(T&					object):
    elle::Derivable<T>(Nucleus::Factory, object)
  {
  }

}

#endif
