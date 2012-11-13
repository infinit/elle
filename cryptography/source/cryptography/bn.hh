#ifndef CRYPTOGRAPHY_BN_HH
# define CRYPTOGRAPHY_BN_HH

# include <iostream>

# include <openssl/bn.h>

/*----------.
| Operators |
`----------*/

std::ostream&
operator <<(std::ostream& stream,
            ::BIGNUM const& bignum);

#endif
