#include <athena/paxos/Client.hh>

namespace athena
{
  namespace paxos
  {
    static const elle::serialization::Hierarchy<elle::Exception>::
    Register<TooFewPeers> _register_serialization;
  }
}
