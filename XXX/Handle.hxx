//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sun mar 11 15:00:10 2012]
//

#ifndef NUCLEUS_PROTON_HANDLE_HXX
#define NUCLEUS_PROTON_HANDLE_HXX

using namespace nucleus::proton;

//
// ---------- methods ---------------------------------------------------------
//

// XXX use static_cast?

///
/// XXX
///
template <typename T>
elle::Status            Handle::Use(T*&                         object)
{
  if ((object = dynamic_cast<T*>(this->_block)) == nullptr)
    escape("unable to cast the block");

  return elle::StatusOk;
}

#endif
