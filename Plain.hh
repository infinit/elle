#ifndef ELLE_CRYPTOGRAPHY_PLAIN_HH
# define ELLE_CRYPTOGRAPHY_PLAIN_HH

# include <elle/standalone/Region.hh>

namespace elle
{
  using namespace standalone;

  namespace cryptography
  {

    ///
    /// this class represents plain data that can be used for cryptographic
    /// purposes.
    ///
    typedef elle::standalone::Region      Plain;

  }
}

#endif
#include <elle/cryptography/Plain.hxx>
