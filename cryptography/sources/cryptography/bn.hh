#ifndef INFINIT_CRYPTOGRAPHY_BN_HH
# define INFINIT_CRYPTOGRAPHY_BN_HH

# include <iostream>

# include <openssl/bn.h>

/*----------.
| Operators |
`----------*/

std::ostream&
operator <<(std::ostream& stream,
            ::BIGNUM const& bignum);

# include <cryptography/bn.hxx>

#endif
