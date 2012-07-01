#ifndef ELLE_NETWORK_OUTPUTS_HXX
# define ELLE_NETWORK_OUTPUTS_HXX

# include <elle/network/Bundle.hh>
# include <elle/network/Message.hh>

namespace elle
{
  namespace network
  {

    ///
    /// this function generates an outputs bundle instance without having
    /// to specify the parameter types.
    ///
    /// this function is being inlined in order to avoid copying the
    /// Bundle instance for nothing.
    ///
    template <const Tag G,
              typename... T>
    inline
    typename Message<G>::B::Outputs     Outputs(T&...           objects)
    {
      return (typename Message<G>::B::Outputs(objects...));
    }

  }
}

#endif
