#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX

# include <elle/Buffer.hh>

namespace infinit
{
  namespace cryptography
  {
    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    Cipher
    SecretKey::encrypt(T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
      ELLE_DEBUG_FUNCTION(value);

      static_assert(!std::is_same<T, Plain>::value,
                    "this call should never have occured");

      // Serialize the value.
      elle::Buffer buffer;
      buffer.writer() << value;

      // Encrypt the archive.
      return (this->encrypt(Plain{elle::WeakBuffer{buffer}}));
    }

    template <typename T>
    T
    SecretKey::decrypt(Cipher const& cipher) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
      ELLE_DEBUG_FUNCTION(cipher);

      static_assert(!std::is_same<T, Clear>::value,
                    "this call should never have occured");

      // Decrypt the cipher leading to a clear containing an archive.
      Clear clear{this->decrypt(cipher)};

      // Deserialize the object from the clear.
      // XXX[this is should be used] T value{clear.buffer().reader()};
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
                      format)
{
  enforce(format == 0);

  archive & value._buffer;
}

#endif
