#ifndef ELLE_THIRDPARTY_EPTH_HH
#define ELLE_THIRDPARTY_EPTH_HH

///
/// epth stands for extended Pth, Pth being a portable cooperative thread.
///
/// since Infinit relies on cooperative thread, aka fibers, the Pth library
/// has been used, especially for handling UNIX operating systems such
/// as Linux, *BSD or even MacOS X.
///
/// however, since Windows is not included in the scope of Pth, a wrapper,
/// i.e epth, has been developed so as to called specific native functions
/// when it comes to Windows platforms.
///

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>

#include <elle/types.hh>


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

elle::Status     epth_initialize(const elle::Natural32,
                                        t_epth&);
elle::Status     epth_clean();
elle::Status     epth_spawn(void* (*)(void*),
                                   void*,
                                   t_epth&);
elle::Status     epth_switch(const t_epth&,
                                    const t_epth&);
elle::Status     epth_abort(const t_epth&);

#endif
