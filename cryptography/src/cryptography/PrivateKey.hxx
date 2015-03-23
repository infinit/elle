#ifndef INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HXX

# include <elle/Buffer.hh>
# include <elle/log.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    T
    PrivateKey::decrypt(Code const& code) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.PrivateKey");
      ELLE_DEBUG_FUNCTION(code);

      static_assert(std::is_same<T, Clear>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::Buffer>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                    "this call should never have occured");

      Clear clear(this->decrypt(code));

      // XXX[this is the way it should be] T value(clear.buffer().reader());
      T value;
      clear.buffer().reader() >> value;

      return (value);
    }

    template <typename T>
    Signature
    PrivateKey::sign(T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.PrivateKey");
      ELLE_DEBUG_FUNCTION(value);

      static_assert(std::is_same<T, Digest>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, Plain>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::Buffer>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                    "this call should never have occured");

      elle::Buffer buffer;
      buffer.writer() << value;

      return (this->sign(Plain(elle::WeakBuffer(buffer))));
    }
  }
}

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>
# include <elle/utility/Factory.hh>

# include <cryptography/KeyPair.hh>
# include <cryptography/Cryptosystem.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::PrivateKey);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::PrivateKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  archive << value._implementation->cryptosystem();

  // XXX[alive_pointer?]
  archive << *value._implementation;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::PrivateKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  infinit::cryptography::Cryptosystem cryptosystem;
  archive >> cryptosystem;

  ELLE_ASSERT_EQ(value._implementation, nullptr);
  value._implementation.reset(
    infinit::cryptography::privatekey::factory().allocate<
      infinit::cryptography::privatekey::Interface>(cryptosystem));

  // XXX[improve by passing the archive to the factory]
  archive >> *value._implementation;
}

//
// ---------- Interface -------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::privatekey::Interface,
                      archive,
                      value,
                      format)
{
  enforce(format == 0);

  (void)archive;
  (void)value;
}

#endif
