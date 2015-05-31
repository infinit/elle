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

    elle::ConstWeakBuffer
    serialize(Plain const& plain)
    {
      ELLE_DEBUG_FUNCTION("");
      ELLE_DUMP("plain: %x", plain);

      return (plain.buffer());
    }
  }
}
