#ifndef COMET_COMET_HH
# define COMET_COMET_HH

///
/// this namespace contains slightly modified OpenSSL functions.
///
/// these functions, unlike OpenSSL ones, enable RSA keys to be rotated. for
/// this purpose, the public exponent 'e' must be deterministically generated
/// according to a given seed.
///
/// Unfortunately, the OpenSSL random generator introduces entropy in many
/// places. therefore, many functions have been cloned and adapted in order
/// to guarantee determinism whenever necessary.
///
namespace comet
{
}

#include <comet/BN.hh>
#include <comet/RAND.hh>
#include <comet/RSA.hh>

#endif
