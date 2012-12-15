#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX

# include <elle/Buffer.hh>

# include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {
    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    Cipher
    SecretKey::encrypt(T const& plain) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
      ELLE_TRACE_METHOD(plain);

      // Serialize the plain object.
      elle::Buffer buffer;
      buffer.writer() << plain;

      // Encrypt the archive.
      return (this->encrypt(Plain{elle::WeakBuffer{buffer}}));
    }

    template <typename T>
    T
    SecretKey::decrypt(Cipher const& cipher) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
      ELLE_TRACE_METHOD(cipher);

      // Decrypt the cipher leading to a clear containing an archive.
      Clear clear{this->decrypt(cipher)};

      // Deserialize the object from the clear.
      T value;
      clear.buffer().reader() >> value;

      return (value);
    }
  }
}

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::SecretKey,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._buffer;
}

#endif
