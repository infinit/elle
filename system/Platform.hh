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

//
// ---------- portability -----------------------------------------------------
//

# ifdef __WIN32__
#  define INFINIT_WIN32 1
# else
#  define INFINIT_WIN32 0
# endif

# ifdef __unix__
#  define INFINIT_UNIX 1
# else
#  define INFINIT_UNIX 0
# endif

#endif
