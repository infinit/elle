//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        Alexandre Bique [thu sep 15 10:58:52 2011]
//

#ifndef ELLE_SYSTEM_PLATFORM_HH
# define ELLE_SYSTEM_PLATEFORM_HH

namespace elle
{
  namespace system
  {

//
// ---------- macros ----------------------------------------------------------
//

# if defined(__WIN32__)
#  define INFINIT_WIN32
# endif

# if defined(__unix__)
#  define INFINIT_UNIX
# endif

  }
}

#endif
