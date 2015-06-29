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

#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
#  include <cryptography/fwd.hh>

#  include <elle/types.hh>
#  include <elle/Buffer.hh>

#  include <iosfwd>

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/

    /// Do nothing and return the plain text received as argument.
    elle::Buffer
    serialize(Plain const& plain);
    /// Serialize the argument and return an archive i.e buffer.
    template <typename T>
    elle::Buffer
    serialize(T const& value);
    /// Deserialize the given T from the buffer.
    template <typename T>
    T
    deserialize(elle::ConstWeakBuffer const& archive);
    /// A specialization for raw data i.e clear.
    template <>
    Clear
    deserialize<Clear>(elle::ConstWeakBuffer const& archive);
  }
}

# endif

# include <cryptography/serialization.hxx>

#endif
