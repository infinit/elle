#ifndef ELLE_CRYPTOGRAPHY_SERIALIZATION_HH
# define ELLE_CRYPTOGRAPHY_SERIALIZATION_HH

# include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/

    /// This function is used by low-level POD types.
    template <typename S>
    void
    serialize(elle::serialization::Serializer& serializer,
              typename S::Type*& data);
  }
}

# include <elle/cryptography/serialization.hxx>

#endif
