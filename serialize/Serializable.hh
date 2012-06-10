#ifndef  ELLE_SERIALIZE_SERIALIZABLE_HH
# define ELLE_SERIALIZE_SERIALIZABLE_HH

# include <elle/concept/Serializable.hh>

namespace elle
{
  namespace serialize
  {

    /// Wrapper for types that that implement Serializable concept
    template<typename T> struct Serializable
      {
        T& value;
        Serializable(T& value) : value(value) {}
        Serializable(Serializable&& o) : value(o.value) {}
      };

    /// Helper to infer the right Serializable type.
    template<typename T> static inline
      Serializable<T> serializable(T& value)
      {
        return Serializable<T>(value);
      }
    template<typename T> static inline
      Serializable<T const> serializable(T const& value)
      {
        return Serializable<T const>(value);
      }

  }
}

# include "Serializable.hxx"

#endif

