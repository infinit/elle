#ifndef ELLE_SERIALIZE_CONCRETE_HH
# define ELLE_SERIALIZE_CONCRETE_HH

# include "detail/wrapper.hh"

///
/// Declare a wrapper named Concrete<T> used to specify (or force) the
/// serialization of the concrete type, even if the type is polymorphic.
///

_ELLE_SERIALIZE_WRAPPER(Concrete, concrete);

#endif
