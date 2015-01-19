#include <cryptography/bn.hh>

#include <iostream>
#include <cstring>

#include <openssl/err.h>

#include <elle/types.hh>

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

/*--------------.
| Serialization |
`--------------*/

namespace elle
{
  namespace serialization
  {
    std::string
    Serialize<BIGNUM*>::convert(BIGNUM*& bignum)
    {
      std::unique_ptr<char, void (*) (char*)> hexadecimal
        (::BN_bn2hex(bignum), [] (char* p) {OPENSSL_free(p);});
      if (!hexadecimal)
        throw ::elle::Error(
          ::elle::sprintf(
            "unable to convert big number to hexadecimal: %s",
            ::ERR_error_string(ERR_get_error(), nullptr)));
      std::string res(hexadecimal.get());
      return res;
    }

    BIGNUM*
    Serialize<BIGNUM*>::convert(std::string& repr)
    {
      BIGNUM* res = nullptr;
      if (BN_hex2bn(&res, repr.c_str()) == 0)
        throw ::elle::Error(
          ::elle::sprintf("unable to read big number from hexadecimal: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
      return res;
    }
  }
}
