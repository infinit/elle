#pragma once

#if defined _MSC_VER
# define ELLE_API __declspec(dllexport)
# define ELLE_COMPILER_ALIGN(Alignment)        __declspec(align(Alignment))
# define ELLE_COMPILER_ATTRIBUTE_ALWAYS_INLINE
# define ELLE_COMPILER_ATTRIBUTE_CONST
# define ELLE_COMPILER_ATTRIBUTE_MAYBE_UNUSED
# define ELLE_COMPILER_ATTRIBUTE_NORETURN      __declspec(noreturn)
# define ELLE_COMPILER_ATTRIBUTE_NO_INLINE     __declspec(noinline)
# define ELLE_COMPILER_ATTRIBUTE_PURE
# define ELLE_COMPILER_PRETTY_FUNCTION         __FUNCSIG__
# define constexpr
#else // Gcc and clang.
# define ELLE_API __attribute__ ((visibility ("default")))
# define ELLE_COMPILER_ALIGN(Alignment)        __attribute__ ((aligned(Alignment)))
# define ELLE_COMPILER_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
# define ELLE_COMPILER_ATTRIBUTE_CONST         __attribute__((const))
# define ELLE_COMPILER_ATTRIBUTE_MAYBE_UNUSED  __attribute__((unused))
# define ELLE_COMPILER_ATTRIBUTE_NORETURN      __attribute__((noreturn))
# define ELLE_COMPILER_ATTRIBUTE_NO_INLINE     __attribute__((noinline))
# define ELLE_COMPILER_ATTRIBUTE_PURE          __attribute__((pure))
# define ELLE_COMPILER_PRETTY_FUNCTION         __PRETTY_FUNCTION__
#endif

#if defined ELLE_DISABLE_VISIBILITY
# undef ELLE_API
# define ELLE_API
#endif

#if defined __GNUC__ && !defined __clang__
# define GCC_VERSION_LTE(Major, Minor)                          \
  ((__GNUC__) < (Major)                                         \
   || (__GNUC__) == (Major) && (__GNUC_MINOR__) <= (Minor))
#else
# define GCC_VERSION_LTE(Major, Minor) 0
#endif
