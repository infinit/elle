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

# include <elle/cryptography/rsa/fwd.hh>
# include <elle/cryptography/dsa/fwd.hh>
# include <elle/cryptography/dh/fwd.hh>

#endif
