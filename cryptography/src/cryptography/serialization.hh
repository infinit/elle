#ifndef INFINIT_CRYPTOGRAPHY_SERIALIZATION_HH
# define INFINIT_CRYPTOGRAPHY_SERIALIZATION_HH

# include <elle/serialization/Serializer.hh>

namespace infinit
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

# include <cryptography/serialization.hxx>

#endif
