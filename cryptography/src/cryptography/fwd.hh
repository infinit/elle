#ifndef ELLE_CRYPTOGRAPHY_FWD_HH
# define ELLE_CRYPTOGRAPHY_FWD_HH

namespace elle
{
  namespace cryptography
  {
    enum class Cipher;
    enum class Mode;
    enum class Cryptosystem;
    enum class Oneway;
    class Error;
    class SecretKey;
  }
}

# include <cryptography/rsa/fwd.hh>
# include <cryptography/dsa/fwd.hh>
# include <cryptography/dh/fwd.hh>

#endif
