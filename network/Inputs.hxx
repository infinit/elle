#ifndef ELLE_NETWORK_INPUTS_HXX
# define ELLE_NETWORK_INPUTS_HXX

# include <elle/network/Bundle.hh>
# include <elle/network/Message.hh>

namespace elle
{
  namespace network
  {

    ///
    /// this function generates an inputs bundle instance without having
    /// to specify the parameter types.
    ///
    /// this function is being inlined in order to avoid copying the
    /// Bundle instance for nothing.
    ///
    template <const Tag G,
              typename... T>
    inline
    typename Message<G>::B::Inputs      Inputs(const T&...      objects)
    {
      return (typename Message<G>::B::Inputs(objects...));
    }

  }
}

#endif
