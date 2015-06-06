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

    elle::Buffer
    serialize(Plain const& plain)
    {
      ELLE_DEBUG_FUNCTION("");
      ELLE_DUMP("plain: %x", plain);

      return (elle::Buffer(plain.buffer().contents(), plain.buffer().size()));
    }
  }
}
