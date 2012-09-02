#ifndef  ELLE_SERIALIZE_IDENTITY_HH
# define ELLE_SERIALIZE_IDENTITY_HH


///
/// Identity wrapper is a "does nothing" wrapper. It just forwards the
/// serialization call to its wrapped value. It is usefull when wrapping is
/// parameterizable.
///

# include "detail/wrapper.hh"

_ELLE_SERIALIZE_WRAPPER(Identity, identity);

# include "Identity.hxx"

#endif
