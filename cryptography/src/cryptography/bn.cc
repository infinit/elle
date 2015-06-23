#include <cryptography/bn.hh>

#include <openssl/bn.h>
#include <openssl/err.h>

#include <iostream>
#include <cstring>

#include <elle/types.hh>
#include <elle/Buffer.hh>

#include <cryptography/finally.hh>
#include <cryptography/Exception.hh>

/*----------.
| Operators |
`----------*/

std::ostream&
operator <<(std::ostream& stream,
            BIGNUM const& bignum)
{
  static elle::Natural32 const length = 50;
  char* hexadecimal;

  // Transform the number in hexadecimal.
  if ((hexadecimal = ::BN_bn2hex(&bignum)) == nullptr)
    throw infinit::cryptography::Exception(
      elle::sprintf("unable to convert the big number into an hexadecimal "
                    "representation: %s",
                    ::ERR_error_string(ERR_get_error(), nullptr)));

  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(hexadecimal);

  // Display the string, depending on its length.
  if (::strlen(hexadecimal) < length)
  {
    // If the string is short enough, display it in its entirety.
    stream << hexadecimal;
  }
  else
  {
    elle::String string(hexadecimal);

    // Otherwise chop it and display the begining and the end only.
    stream << string.substr(0, length / 2)
           << "..." << std::dec << BN_num_bytes(&bignum)
           << " bytes" << "..."
           << string.substr(string.length() - (length / 2));
  }

  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(hexadecimal);

  // Manually release the hexadecimal's memory.
  ::OPENSSL_free(hexadecimal);

  return (stream);
}

elle::Boolean
operator ==(::BIGNUM const& a,
            ::BIGNUM const& b)
{
  return (::BN_cmp(&a, &b) == 0);
}

elle::Boolean
operator <(::BIGNUM const& a,
           ::BIGNUM const& b)
{
  return (::BN_cmp(&a, &b) < 0);
}

/*--------------.
| Serialization |
`--------------*/

namespace elle
{
  namespace serialization
  {
    elle::Buffer
    Serialize<BIGNUM*>::convert(::BIGNUM* const& value)
    {
      elle::Buffer representation(BN_num_bytes(value));

      if (representation.size() <= 0)
        throw infinit::cryptography::Exception(
          elle::sprintf("unable to save an empty big number"));

      if (::BN_bn2bin(value, representation.mutable_contents()) <= 0)
        throw infinit::cryptography::Exception(
          elle::sprintf("unable to serialize the big number: %s",
                        ::ERR_error_string(ERR_get_error(), nullptr)));

      return (representation);
    }

    ::BIGNUM*
    Serialize<BIGNUM*>::convert(elle::Buffer const& representation)
    {
      if (representation.size() <= 0)
        throw infinit::cryptography::Exception(
          elle::sprintf("unable to load an empty big number"));

      ::BIGNUM* value = ::BN_new();

      INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(value);

      if (::BN_bin2bn(representation.contents(),
                      static_cast<int>(representation.size()),
                      value) == nullptr)
        throw infinit::cryptography::Exception(
          elle::sprintf("unable to convert the binary data into a big "
                        "number: %s",
                        ::ERR_error_string(ERR_get_error(), nullptr)));

      INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(value);

      return (value);
    }
  }
}
