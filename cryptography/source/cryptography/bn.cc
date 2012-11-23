#include <cryptography/bn.hh>
#include <cryptography/finally.hh>

#include <elle/types.hh>
#include <elle/Exception.hh>

# include <openssl/err.h>

std::ostream&
operator <<(std::ostream& stream,
            ::BIGNUM const& bignum)
{
  static elle::Natural32 const length = 50;
  char* hexadecimal;

  // Transform the number in hexadecimal.
  if ((hexadecimal = ::BN_bn2hex(&bignum)) == nullptr)
    throw elle::Exception("%s",
                          ::ERR_error_string(ERR_get_error(), nullptr));

  CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(hexadecimal);

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
      stream << string.substr(0, length / 2) << "..."
             << string.substr(string.length() - (length / 2));
    }

  CRYPTOGRAPHY_FINALLY_ABORT(hexadecimal);

  // Manually release the hexadecimal's memory.
  ::OPENSSL_free(hexadecimal);

  return (stream);
}
