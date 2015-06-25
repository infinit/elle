#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)

# include <cryptography/serialization.hh>
# include <cryptography/Plain.hh>

# include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.cryptography");

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/

    /// WARNING: this is not optimized as the buffer is cloned! To be
    ///          improved through streams.
    elle::Buffer
    serialize(Plain const& plain)
    {
      ELLE_DEBUG_FUNCTION("");
      ELLE_DUMP("plain: %x", plain);

      return (elle::Buffer(plain.buffer().contents(), plain.buffer().size()));
    }

    /// WARNING: this is not optimized as the buffer is cloned! To be
    ///          improved through streams.
    template <>
    Clear
    deserialize<Clear>(elle::ConstWeakBuffer const& archive)
    {
      elle::Buffer buffer(archive.contents(), archive.size());

      return (Clear(buffer));
    }
  }
}

#endif
