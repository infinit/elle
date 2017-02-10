#ifndef ELLE_CRYPTOGRAPHY_RSA_DEFAULTS_HH
# define ELLE_CRYPTOGRAPHY_RSA_DEFAULTS_HH

# include <cryptography/rsa/Padding.hh>
# include <cryptography/Oneway.hh>
# include <cryptography/Cipher.hh>

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace defaults
      {
        /*---------------.
        | Default Values |
        `---------------*/

        static Padding const encryption_padding = Padding::oaep;
        static Padding const signature_padding = Padding::pss;
        static Oneway const oneway = Oneway::sha256;
        static Cipher const envelope_cipher = Cipher::aes256;
        static Mode const envelope_mode = Mode::cbc;
      }
    }
  }
}

#endif
