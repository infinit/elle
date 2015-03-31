#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX

# include <elle/Buffer.hh>
# include <elle/log.hh>

# include <cryptography/Plain.hh>

namespace infinit
{
  namespace cryptography
  {
    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    Code
    SecretKey::encrypt(T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
      ELLE_DEBUG_FUNCTION(value);

      static_assert(std::is_same<T, Plain>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::Buffer>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                    "this call should never have occured");

      // Serialize the value.
      elle::Buffer buffer;
      buffer.writer() << value;

      // Encrypt the archive.
      return (this->encrypt(Plain(buffer)));
    }

    template <typename T>
    T
    SecretKey::decrypt(Code const& code) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.SecretKey");
      ELLE_DEBUG_FUNCTION(code);

      static_assert(std::is_same<T, Clear>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::Buffer>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                    "this call should never have occured");

      // Decrypt the code leading to a clear containing an archive.
      Clear clear(this->decrypt(code));

      // Deserialize the object from the clear.
      // XXX[this is should be used] T value(clear.buffer().reader());
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

  archive & value._password;
  archive & value._cipher;
  archive & value._mode;
  archive & value._oneway;
}

#endif
