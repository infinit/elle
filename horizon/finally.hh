#ifndef HORIZON_FINALLY_HH
# define HORIZON_FINALLY_HH

# include <elle/finally.hh>

/// Make it easy to release an scope from etoile.
# define HORIZON_FINALLY_DISCARD(_variable_)                            \
  ELLE_FINALLY_LAMBDA(                                                  \
    _variable_,                                                         \
    [] (etoile::gear::Identifier const& identifier)                     \
    {                                                                   \
      etoile::wall::Object::discard(identifier);                        \
    });

/// Make it super easy to abort the final action based on the name of
/// the variable it relates to.
# define HORIZON_FINALLY_ABORT(_variable_)                              \
  ELLE_FINALLY_ABORT(_variable_)

#endif
