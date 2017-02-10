#ifndef ELLE_CRYPTOGRAPHY_DSA_DEFAULTS_HH
# define ELLE_CRYPTOGRAPHY_DSA_DEFAULTS_HH

# include <cryptography/Oneway.hh>

namespace elle
{
  namespace cryptography
  {
    namespace dsa
    {
      namespace defaults
      {
        /*---------------.
        | Default Values |
        `---------------*/

        static Oneway const digest_algorithm = Oneway::sha256;
      }
    }
  }
}

#endif
