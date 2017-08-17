#pragma once
/* This file needs to be included exactly once per dll and executable that links with
 * libelle_reactor.dll under windows.
*/
#ifdef ELLE_WINDOWS
# include <elle/reactor/libcxx-exceptions/unwind-cxx.h>
namespace elle
{
  namespace reactor
  {
    __cxxabiv1::__cxa_eh_globals*
    cxa_get_globals() throw();
  }
}
namespace __cxxabiv1
{
  extern "C"
  { 
    __cxa_eh_globals *
    __cxa_get_globals() throw()
    { 
      return elle::reactor::cxa_get_globals();
    }   
        __cxa_eh_globals *
    __cxa_get_globals_fast() throw()
    { 
      return __cxa_get_globals();
    }
  }
}
#endif

