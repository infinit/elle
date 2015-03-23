#ifndef INFINIT_CRYPTOGRAPHY_PUBLICKEY_HXX
# define INFINIT_CRYPTOGRAPHY_PUBLICKEY_HXX

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
    Code
    PublicKey::encrypt(T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.PublicKey");
      ELLE_DEBUG_FUNCTION(value);

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

      return (this->encrypt(Plain(elle::WeakBuffer(buffer))));
    }

    template <typename T>
    elle::Boolean
    PublicKey::verify(Signature const& signature,
                      T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.PublicKey");
      ELLE_DEBUG_FUNCTION(signature, value);

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

      return (this->verify(signature, Plain(elle::WeakBuffer(buffer))));
    }
  }
}

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>
# include <elle/utility/Factory.hh>

# include <cryptography/Cryptosystem.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::PublicKey);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::PublicKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  archive << value._implementation->cryptosystem();

  // XXX[alive_pointer?]
  archive << *value._implementation;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::PublicKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  infinit::cryptography::Cryptosystem cryptosystem;
  archive >> cryptosystem;

  ELLE_ASSERT_EQ(value._implementation, nullptr);
  value._implementation.reset(
    infinit::cryptography::publickey::factory().allocate<
      infinit::cryptography::publickey::Interface>(cryptosystem));

  // XXX[improve by passing the archive to the factory]
  archive >> *value._implementation;
}

//
// ---------- Interface -------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::publickey::Interface,
                      archive,
                      value,
                      format)
{
  enforce(format == 0);

  (void)archive;
  (void)value;
}

#endif
