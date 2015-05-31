#ifndef INFINIT_CRYPTOGRAPHY_SERIALIZATION_HH
# define INFINIT_CRYPTOGRAPHY_SERIALIZATION_HH

# include <cryptography/fwd.hh>

# include <elle/types.hh>
# include <elle/Buffer.hh>

# include <iosfwd>

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/

    /// Do nothing and return the plain text received as argument.
    elle::ConstWeakBuffer
    serialize(Plain const& plain);
    /// Serialize the argument and return an archive i.e buffer.
    template <typename T>
    elle::ConstWeakBuffer
    serialize(T const& value);
  }
}

# include <cryptography/serialization.hxx>

#endif
