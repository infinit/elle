#ifndef CRYPTOGRAPHY_FINALLY_HH
# define CRYPTOGRAPHY_FINALLY_HH

# include <elle/finally.hh>

# include <boost/preprocessor/cat.hpp>

/// Provide a lambda-based skeleton for creating Finally instances.
# define CRYPTOGRAPHY_FINALLY_LAMBDA(_variable_, _lambda_)              \
  auto BOOST_PP_CAT(_elle_finally_lambda_, __LINE__) = _lambda_;        \
  elle::Finally BOOST_PP_CAT(_cryptography_finally_, _variable_)(       \
    std::bind(BOOST_PP_CAT(_elle_finally_lambda_, __LINE__), _variable_));

/// Make it so simple to track an EVP_PKEY so it gets freed should an
/// error occur i.e should we leave the scope.
# define CRYPTOGRAPHY_FINALLY_FREE_EVP_PKEY(_variable_)                 \
  CRYPTOGRAPHY_FINALLY_LAMBDA(                                          \
    _variable_,                                                         \
    [] (::EVP_PKEY* pkey) { ::EVP_PKEY_free(pkey); });

/// Make it easy to free an RSA key on leaving the scope.
# define CRYPTOGRAPHY_FINALLY_FREE_RSA(_variable_)                      \
  CRYPTOGRAPHY_FINALLY_LAMBDA(                                          \
    _variable_,                                                         \
    [] (::RSA* rsa) { ::RSA_free(rsa); });

/// Make it simply to free a BINUM.
# define CRYPTOGRAPHY_FINALLY_FREE_BN(_variable_)                       \
  CRYPTOGRAPHY_FINALLY_LAMBDA(                                          \
    _variable_,                                                         \
    [] (::BIGNUM* bignum) { ::BN_clear_free(bignum); });

/// Make it easy to free memory through the OpenSSL_free() function.
# define CRYPTOGRAPHY_FINALLY_FREE(_variable_)                          \
  CRYPTOGRAPHY_FINALLY_LAMBDA(                                          \
    _variable_,                                                         \
    [] (void* pointer) { ::OPENSSL_free(pointer); });

/// Make it super easy to abort the final action based on the name of
/// the variable it relates to.
# define CRYPTOGRAPHY_FINALLY_ABORT(_variable_)                         \
  BOOST_PP_CAT(_cryptography_finally_, _variable_).abort();

#endif
