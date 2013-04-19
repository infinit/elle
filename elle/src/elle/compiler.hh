#ifndef ELLE_COMPILER_HH
# define ELLE_COMPILER_HH

# define ELLE_COMPILER_ATTRIBUTE_NORETURN __attribute__((noreturn))
# define ELLE_COMPILER_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
# define ELLE_COMPILER_ATTRIBUTE_NO_INLINE __attribute__((noinline))
# define ELLE_COMPILER_PRETTY_FUNCTION __PRETTY_FUNCTION__

#endif
