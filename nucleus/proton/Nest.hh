#ifndef NUCLEUS_PROTON_NEST_HH
# define NUCLEUS_PROTON_NEST_HH

# include <elle/types.hh>
# include <elle/attribute.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Limits.hh>
# include <nucleus/proton/Network.hh>

# include <cryptography/PublicKey.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

namespace nucleus
{
  namespace proton
  {
    /// XXX
    class Nest
    {
      // XXX
    public:
      Nest(Limits const& limits,
           Network const& network,
           cryptography::PublicKey const& agent_K);
      virtual
      ~Nest() = default;

      //
      // methods
      //
    public:
      /// XXX: ownership is transferred
      virtual
      Handle
      attach(Contents* block) = 0;
      /// XXX
      virtual
      void
      detach(Handle& handle) = 0;
      /// XXX
      virtual
      void
      load(Handle& handle) = 0;
      /// XXX
      virtual
      void
      unload(Handle& handle) = 0;

      //
      // attributes
      //
    private:
      ELLE_ATTRIBUTE_R(nucleus::proton::Limits, limits);
      ELLE_ATTRIBUTE_R(Network, network); // XXX
      ELLE_ATTRIBUTE_R(cryptography::PublicKey, agent_K); // XXX
    };
  }
}

#endif
