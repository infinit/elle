//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Thu Jan 22 16:27:34 2009 texane
// Last update Sun Jan 25 10:12:42 2009 texane
//



#ifndef DEBUG_HH_INCLUDED
# define DEBUG_HH_INCLUDED



#if _DEBUG

# ifdef _MSC_VER

# include <cstdio>

#pragma warning(disable: 4127)

# define DEBUG_PRINTF(S, ...) do { ::printf("[?] " __FUNCTION__ ": " S, __VA_ARGS__); } while (0)
# define DEBUG_ERROR(S, ...) do { ::printf("[!] " __FUNCTION__ ": " S, __VA_ARGS__); } while (0)
# define DEBUG_ENTER() do { ::printf("[>] " __FUNCTION__ "\n"); } while (0)
# define DEBUG_LEAVE() do { ::printf("[<] " __FUNCTION__ "\n"); } while (0)
# define DEBUG_ASSERT(C) do { if (!(C)) ::printf("[!] " __FUNCTION__ ": assert(" #C ")\n"); } while (0)

#else /* ! _MSC_VER */

# include <cstdio>

# define DEBUG_PRINTF(S, ...) do { ::printf("[?] %s: " S, __func__, ##__VA_ARGS__); } while (0)
# define DEBUG_ERROR(S, ...) do { ::printf("[!] %s: " S, __func__, ##__VA_ARGS__); } while (0)
# define DEBUG_ENTER() do { ::printf("[>] %s\n", __func__); } while (0)
# define DEBUG_LEAVE() do { ::printf("[<] %s\n", __func__); } while (0)
# define DEBUG_ASSERT(C) do { if (!(C)) ::printf("[!] %s: assert(" #C ")\n", __func__); } while (0)

#endif /* _MSC_VER */

#else /* !  _DEBUG */

# define DEBUG_PRINTF(fmt, ...) do {} while (0)
# define DEBUG_ERROR(s, ...) do {} while (0)
# define DEBUG_ENTER() do {} while (0)
# define DEBUG_LEAVE() do {} while (0)
# define DEBUG_ASSERT( c ) do {} while (0)

#endif /* _DEBUG */



#endif /* ! DEBUG_HH_INCLUDED */
