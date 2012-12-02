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
      /// XXX
      extern Flags const none;
      /// XXX
      extern Flags const recursive;
      /// XXX
      extern Flags const key;
      /// XXX
      extern Flags const address;
      /// XXX
      extern Flags const capacity;
      /// XXX
      extern Flags const footprint;
      /// XXX
      extern Flags const state;
      /// XXX
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
