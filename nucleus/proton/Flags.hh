#ifndef NUCLEUS_PROTON_FLAGS_HH
# define NUCLEUS_PROTON_FLAGS_HH

# include <elle/types.hh>

namespace nucleus
{
  namespace proton
  {
    /*------.
    | Types |
    `------*/

    /// Define the type for representing the flags used for checking
    /// the integrity of a tree-based data structure.
    typedef elle::Natural16 Flags;

    /*-------.
    | Values |
    `-------*/

    namespace flags
    {
      extern Flags const none;
      extern Flags const recursive;
      extern Flags const key;
      extern Flags const address;
      extern Flags const capacity;
      extern Flags const footprint;
      extern Flags const state;
      extern Flags const all;
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Flags const flags);
  }
}

#endif
