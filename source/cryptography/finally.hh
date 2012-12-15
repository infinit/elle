#ifndef CRYPTOGRAPHY_FINALLY_HH
# define CRYPTOGRAPHY_FINALLY_HH

# include <elle/finally.hh>

/// Make it so simple to track an EVP_PKEY so it gets freed should an
/// error occur i.e should we leave the scope.
# define CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(_variable_)          \
  ELLE_FINALLY_LAMBDA(                                                  \
    _variable_,                                                         \
    [] (::EVP_PKEY* pkey) { ::EVP_PKEY_free(pkey); });

/// Make it easy to free an RSA key on leaving the scope.
# define CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(_variable_)               \
  ELLE_FINALLY_LAMBDA(                                                  \
    _variable_,                                                         \
    [] (::RSA* rsa) { ::RSA_free(rsa); });

/// Make it simply to free a BINUM.
# define CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(_variable_)                \
  ELLE_FINALLY_LAMBDA(                                                  \
    _variable_,                                                         \
    [] (::BIGNUM* bignum) { ::BN_clear_free(bignum); });

/// Make it easy to free memory through the OpenSSL_free() function.
# define CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(_variable_)           \
  ELLE_FINALLY_LAMBDA(                                                  \
    _variable_,                                                         \
    [] (void* pointer) { ::OPENSSL_free(pointer); });

/// Make it easy to clean up a cipher context.
# define CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_CIPHER_CONTEXT(_variable_) \
  ELLE_FINALLY_LAMBDA(                                                  \
    _variable_,                                                         \
    [] (::EVP_CIPHER_CTX& context)                                      \
    {                                                                   \
      ::EVP_CIPHER_CTX_cleanup(&context);                               \
    });

/// Make it super easy to abort the final action based on the name of
/// the variable it relates to.
# define CRYPTOGRAPHY_FINALLY_ABORT(_variable_)                         \
  ELLE_FINALLY_ABORT(_variable_)

#endif
