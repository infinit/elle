#ifndef INFINIT_CRYPTOGRAPHY_CRYPTOSYSTEM_HH
# define INFINIT_CRYPTOGRAPHY_CRYPTOSYSTEM_HH

# include <iosfwd>

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Define the asymmetric cryptosystem algorithm.
    enum class Cryptosystem
    {
      rsa
      // XXX[more even though not supported yet]
    };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Cryptosystem const cryptosystem);
  }
}

#endif
