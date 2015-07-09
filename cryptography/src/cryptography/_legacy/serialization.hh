#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# ifndef INFINIT_CRYPTOGRAPHY_LEGACY_SERIALIZATION_HH
#  define INFINIT_CRYPTOGRAPHY_LEGACY_SERIALIZATION_HH

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

# include <cryptography/_legacy/serialization.hxx>

#else
# warning "LEGACY: this file should not have been included"
#endif
