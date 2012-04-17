#ifndef ELLE_NETWORK_DATA_HH
# define ELLE_NETWORK_DATA_HH

# include <elle/utility/Buffer.hh>

namespace elle
{
  namespace network
  {

    ///
    /// this type represents a message's data section which contains the
    /// arguments.
    ///
    typedef elle::utility::Buffer             Data;

  }
}

#endif
