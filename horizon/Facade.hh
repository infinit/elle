//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// author        julien.quintard   [thu dec 22 16:09:39 2011]
//

#ifndef FACADE_FACADE_HH
#define FACADE_FACADE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>

///
/// XXX
///
namespace facade
{

  // nothing

}

//
// ---------- includes --------------------------------------------------------
//

#if defined(INFINIT_UNIX)
# include <facade/unix/UNIX.hh>
#elif defined(INFINIT_WIN32)
// XXX
#endif

#endif
