#ifndef ELLE_COMPILER_HH
# define ELLE_COMPILER_HH

# ifdef _MSC_VER
#  define ELLE_COMPILER_ATTRIBUTE_NORETURN __declspec(noreturn)
#  define ELLE_COMPILER_ATTRIBUTE_ALWAYS_INLINE
#  define ELLE_COMPILER_ATTRIBUTE_NO_INLINE __declspec((noinline))
#  define ELLE_COMPILER_PRETTY_FUNCTION __FUNCSIG__
#  define ELLE_COMPILER_ALIGN(Alignment) __declspec(align(Alignment
#  define constexpr
# else
#  define ELLE_COMPILER_ATTRIBUTE_NORETURN __attribute__((noreturn))
#  define ELLE_COMPILER_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#  define ELLE_COMPILER_ATTRIBUTE_NO_INLINE __attribute__((noinline))
#  define ELLE_COMPILER_PRETTY_FUNCTION __PRETTY_FUNCTION__
#  define ELLE_COMPILER_ALIGN(Alignment) __attribute__ ((aligned(Alignment)))
# endif

# if defined(__GNUC__) && !defined(__clang__)
#  define GCC_VERSION_LTE(Major, Minor)                                 \
  (__GNUC__) < Major || (__GNUC__) == Major && (__GNUC_MINOR__) <= Minor
#else
# define GCC_VERSION_LTE(Major, Minor) 0
#endif

#endif
