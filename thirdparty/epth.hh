//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu sep 15 15:15:52 2011]
//

#ifndef ELLE_THIRDPARTY_EPTH_HH
#define ELLE_THIRDPARTY_EPTH_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>

//
// ---------- includes --------------------------------------------------------
//

#if defined(INFINIT_LINUX)
# include <elle/thirdparty/epth-linux.hh>
#elif defined (INFINIT_MACOSX)
# include <elle/thirdparty/epth-macosx.hh>
#elif defined(INFINIT_WINDOWS)
# include <elle/thirdparty/epth-windows.hh>
#else
#  error "this platform is not supported"
#endif

//
// ---------- prototypes ------------------------------------------------------
//

elle::radix::Status     epth_initialize(const elle::core::Natural32,
                                        t_epth&);
elle::radix::Status     epth_clean();
elle::radix::Status     epth_spawn(void* (*)(void*),
                                   void*,
                                   t_epth&);
elle::radix::Status     epth_switch(const t_epth&,
                                    const t_epth&);
elle::radix::Status     epth_abort(const t_epth&);

#endif
