#ifndef INFINIT_CRYPTOGRAPHY_DSA_DEFAULTS_HH
# define INFINIT_CRYPTOGRAPHY_DSA_DEFAULTS_HH

# include <cryptography/Oneway.hh>

namespace infinit
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
