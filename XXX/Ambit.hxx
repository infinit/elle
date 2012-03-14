//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [mon mar 12 20:24:05 2012]
//

#ifndef NUCLEUS_PROTON_AMBIT_HXX
#define NUCLEUS_PROTON_AMBIT_HXX

using namespace nucleus::proton;

//
// ---------- constructors & destructors --------------------------------------
//

///
/// XXX
///
/// XXX: manual(manual load/unload), automatic(unload automatic),
///      transparent(both load/unload are automatic)
///
template <typename T>
Ambit<T>::Ambit(Handle&                                         handle,
                Mode                                            mode):
  mode(mode),
  handle(handle)
{
  switch (this->mode)
    {
    case Ambit<T>::ModeTransparent:
      {
        if (this->handle.Load() == elle::StatusError)
          log("an error occured while loading the handle");

        break;
      }
    default:
      {
        // nothing to do.

        break;
      }
    }
}

///
/// XXX
///
template <typename T>
Ambit<T>::~Ambit()
{
  switch (this->mode)
    {
    case Ambit<T>::ModeAutomatic:
    case Ambit<T>::ModeTransparent:
      {
        /* XXX
        if (this->handle.Unload() == elle::StatusError)
          log("an error occured while unloading the handle");
        */

        break;
      }
    default:
      {
        // nothing to do.

        break;
      }
    }
}

//
// ---------- methods ---------------------------------------------------------
//

///
/// XXX
///
template <typename T>
elle::Status    Ambit<T>::Load()
{
  return (this->handle.Load());
}

///
/// XXX
///
template <typename T>
elle::Status    Ambit<T>::Unload()
{
  return (this->handle.Unload());
}

//
// ---------- operators -------------------------------------------------------
//

///
/// XXX
///
template <typename T>
T*              Ambit<T>::operator()()
{
  return (static_cast<T*>(this->handle._block));
}

//
// ---------- dumpable --------------------------------------------------------
//

///
/// XXX
///
template <typename T>
elle::Status        Ambit<T>::Dump(const elle::Natural32        margin) const
{
  elle::String      alignment(margin, ' ');

  std::cout << alignment << "[Ambit]" << std::endl;

  // dump the mode.
  std::cout << alignment << elle::Dumpable::Shift
            << "[Mode] " << std::dec << this->mode << std::endl;

  // dump the handle.
  if (this->handle.Dump(margin + 2) == elle::StatusError)
    escape("unable to dump the handle");

  return elle::StatusOk;
}

#endif
