#include <cryptography/pem.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/finally.hh>
#include <cryptography/rsa/KeyPair.hh>

#include <elle/log.hh>

#include <openssl/evp.h>
#include <openssl/pem.h>

#include <cstdio>

ELLE_LOG_COMPONENT("infinit.cryptography.pem");

namespace infinit
{
  namespace cryptography
  {
    namespace pem
    {
      /*----------.
      | Functions |
      `----------*/

      // XXX we can pass the passphrase directly: https://www.openssl.org/docs/crypto/pem.html
      int
      callback(char* buffer,
               int size,
               int rwflag,
               void *u)
      {
        char* passphrase = (char*)u;
        int length = ::strlen(passphrase);

        // If too long, truncate.
        if (length > size)
          length = size;

        memcpy(buffer, passphrase, length);

        return (length);
      }
    }
  }
}
