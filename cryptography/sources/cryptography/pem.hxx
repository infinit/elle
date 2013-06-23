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
        static_assert(sizeof (T) != 0, "non-supported type");
      }

      template <>
      KeyPair
      load(boost::filesystem::path const& path,
           elle::String const& password)
      {
        return (load_keypair(path, password));
      }

      template <>
      PublicKey
      load(boost::filesystem::path const& path,
           elle::String const& password)
      {
        return (load_K(path, password));
      }

      template <>
      PrivateKey
      load(boost::filesystem::path const& path,
           elle::String const& password)
      {
        return (load_k(path, password));
      }
    }
  }
}

#endif
