#include <elle/athena/paxos/Client.hh>

namespace
{
  template <typename T>
  using register_t = elle::serialization::Hierarchy<elle::Exception>:: Register<T>;

  const auto r1 = register_t<elle::athena::paxos::Unavailable>{};
  const auto r2 = register_t<elle::athena::paxos::TooFewPeers>{};
}
