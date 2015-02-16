#ifndef INFINIT_CRYPTOGRAPHY_PEM_HXX
# define INFINIT_CRYPTOGRAPHY_PEM_HXX

namespace infinit
{
  namespace cryptography
  {
    namespace pem
    {
      /*----------.
      | Functions |
      `----------*/

      template <typename T>
      T
      load(boost::filesystem::path const&,
           elle::String const&)
      {
        static_assert(sizeof (T) == 0,
                      "unsupported type for loading from a PEM file");
        elle::unreachable();
      }

      template <>
      KeyPair
      load(boost::filesystem::path const& path,
           elle::String const& passphrase)
      {
        return (load_keypair(path, passphrase));
      }

      template <>
      PublicKey
      load(boost::filesystem::path const& path,
           elle::String const& passphrase)
      {
        return (load_K(path, passphrase));
      }

      template <>
      PrivateKey
      load(boost::filesystem::path const& path,
           elle::String const& passphrase)
      {
        return (load_k(path, passphrase));
      }
    }
  }
}

#endif
