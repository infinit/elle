#ifndef INFINIT_CRYPTOGRAPHY_FINALLY_HH
# define INFINIT_CRYPTOGRAPHY_FINALLY_HH

# include <elle/finally.hh>

/// Make it so simple to track an EVP_PKEY so it gets freed should an
/// error occur i.e should we leave the scope.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(V)   \
  elle::SafeFinally _finally_##V(                               \
    [&] () { ::EVP_PKEY_free(V); })                             \

/// Make it easy to free an RSA key on leaving the scope.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(V)        \
  elle::SafeFinally _finally_##V(                               \
    [&] () { ::RSA_free(V); });                                 \

/// Make it easy to free an DSA key on leaving the scope.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DSA(V)        \
  elle::SafeFinally _finally_##V(                               \
    [&] () { ::DSA_free(V); });                                 \

/// Make it easy to free an DH key on leaving the scope.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DH(V)         \
  elle::SafeFinally _finally_##V(                               \
    [&] () { ::DH_free(V); });                                  \

/// Make it simply to free a BINUM.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(V) \
  elle::SafeFinally _finally_##V(                       \
    [&] () { ::BN_clear_free(V); });                    \

/// Make it easy to free memory through the OpenSSL_free() function.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_OPENSSL(V)    \
  elle::SafeFinally _finally_##V(                               \
    [&] () { ::OPENSSL_free(V); });                             \

/// Make it easy to clean up a cipher context.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_CIPHER_CONTEXT(V)  \
  elle::SafeFinally _finally_##V(                                       \
    [&] () { ::EVP_CIPHER_CTX_cleanup(&V); });                          \

/// Make it easy to clean up a digest context.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_CLEANUP_DIGEST_CONTEXT(V)  \
  elle::SafeFinally _finally_##V(                                       \
    [&] () { ::EVP_MD_CTX_cleanup(&V); });                              \

/// Make it easy to clean up a EVP_PKEY context.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY_CONTEXT(V)   \
  elle::SafeFinally _finally_##V(                                       \
    [&] () { ::EVP_PKEY_CTX_free(V); });                                \

/// Make it easy to clean up a EVP_PKEY context.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BIO(V)                \
  elle::SafeFinally _finally_##V(                                       \
    [&] () { ::BIO_free(V); });                                         \

/// Make it super easy to abort the final action based on the name of
/// the variable it relates to.
# define INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(V)  \
  _finally_##V.abort()                          \

#endif
