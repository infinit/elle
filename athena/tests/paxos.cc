#include <elle/test.hh>

#include <reactor/Barrier.hh>
#include <reactor/scheduler.hh>
#include <reactor/signal.hh>

#include <athena/paxos/Client.hh>
#include <athena/paxos/Server.hh>

ELLE_LOG_COMPONENT("athena.paxos.test");

namespace paxos = athena::paxos;

namespace std
{
  std::ostream&
  operator <<(std::ostream& output, boost::optional<int> const& v)
  {
    if (v)
      elle::fprintf(output, "Some(%s)", v.get());
    else
      elle::fprintf(output, "None");
    return output;
  }
}

template <typename T, typename ServerId>
class Peer
  : public paxos::Client<T, ServerId>::Peer
{
public:
  Peer(paxos::Server<T, ServerId>& paxos)
    : _paxos(paxos)
  {}

  virtual
  boost::optional<typename paxos::Client<T, ServerId>::Accepted>
  propose(typename paxos::Client<T, ServerId>::Proposal const& p) override
  {
    return this->_paxos.propose(p);
  }

  virtual
  typename paxos::Client<T, ServerId>::Proposal
  accept(typename paxos::Client<T, ServerId>::Proposal const& p,
         T const& value) override
  {
    return this->_paxos.accept(p, value);
  }

  ELLE_ATTRIBUTE_RW((paxos::Server<T, ServerId>&), paxos);
};

ELLE_TEST_SCHEDULED(all_is_well)
{
  paxos::Server<int, int> server_1;
  paxos::Server<int, int> server_2;
  paxos::Server<int, int> server_3;
  typedef paxos::Client<int, int>::Peers Peers;
  Peers peers;
  peers.push_back(elle::make_unique<Peer<int, int>>(server_1));
  peers.push_back(elle::make_unique<Peer<int, int>>(server_2));
  peers.push_back(elle::make_unique<Peer<int, int>>(server_3));
  paxos::Client<int, int> client(1, std::move(peers));
  BOOST_CHECK(!client.choose(42));
}

template <typename T, typename ServerId>
class UnavailablePeer
  : public paxos::Client<T, ServerId>::Peer
{
  virtual
  boost::optional<typename paxos::Client<T, ServerId>::Accepted>
  propose(typename paxos::Client<T, ServerId>::Proposal const& p) override
  {
    throw typename paxos::Client<T, ServerId>::Peer::Unavailable();
  }

  virtual
  typename paxos::Client<T, ServerId>::Proposal
  accept(typename paxos::Client<T, ServerId>::Proposal const& p,
         T const& value) override
  {
    throw typename paxos::Client<T, ServerId>::Peer::Unavailable();
  }
};

ELLE_TEST_SCHEDULED(two_of_three)
{
  paxos::Server<int, int> server_1;
  paxos::Server<int, int> server_2;
  typedef paxos::Client<int, int>::Peers Peers;
  Peers peers;
  peers.push_back(elle::make_unique<Peer<int, int>>(server_1));
  peers.push_back(elle::make_unique<Peer<int, int>>(server_2));
  peers.push_back(elle::make_unique<UnavailablePeer<int, int>>());
  paxos::Client<int, int> client(1, std::move(peers));
  BOOST_CHECK(!client.choose(42));
}

ELLE_TEST_SCHEDULED(one_of_three)
{
  paxos::Server<int, int> server_1;
  typedef paxos::Client<int, int>::Peers Peers;
  Peers peers;
  peers.push_back(elle::make_unique<Peer<int, int>>(server_1));
  peers.push_back(elle::make_unique<UnavailablePeer<int, int>>());
  peers.push_back(elle::make_unique<UnavailablePeer<int, int>>());
  paxos::Client<int, int> client(1, std::move(peers));
  BOOST_CHECK_THROW(client.choose(42), elle::Error);
}

ELLE_TEST_SCHEDULED(already_chosen)
{
  paxos::Server<int, int> server_1;
  paxos::Server<int, int> server_2;
  paxos::Server<int, int> server_3;
  typedef paxos::Client<int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(elle::make_unique<Peer<int, int>>(server_1));
  peers_1.push_back(elle::make_unique<Peer<int, int>>(server_2));
  peers_1.push_back(elle::make_unique<Peer<int, int>>(server_3));
  paxos::Client<int, int> client_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(elle::make_unique<Peer<int, int>>(server_1));
  peers_2.push_back(elle::make_unique<Peer<int, int>>(server_2));
  peers_2.push_back(elle::make_unique<Peer<int, int>>(server_3));
  paxos::Client<int, int> client_2(1, std::move(peers_2));
  BOOST_CHECK(!client_1.choose(42));
  BOOST_CHECK_EQUAL(client_2.choose(43), 42);
}

template <typename T, typename ServerId>
class InstrumentedPeer
  : public Peer<T, ServerId>
{
public:
  InstrumentedPeer(paxos::Server<T, ServerId>& paxos)
    : Peer<T, ServerId>(paxos)
    , fail(false)
  {}

  virtual
  boost::optional<typename paxos::Client<T, ServerId>::Accepted>
  propose(typename paxos::Client<T, ServerId>::Proposal const& p) override
  {
    if (fail)
      throw typename Peer<T, ServerId>::Unavailable();
    this->propose_signal.signal();
    reactor::wait(this->propose_barrier);
    return Peer<T, ServerId>::propose(p);
  }

  virtual
  typename paxos::Client<T, ServerId>::Proposal
  accept(typename paxos::Client<T, ServerId>::Proposal const& p,
         T const& value) override
  {
    if (fail)
      throw typename Peer<T, ServerId>::Unavailable();
    this->accept_signal.signal();
    reactor::wait(this->accept_barrier);
    return Peer<T, ServerId>::accept(p, value);
  }

  bool fail;
  reactor::Barrier propose_barrier, accept_barrier;
  reactor::Signal propose_signal, accept_signal;
};

ELLE_TEST_SCHEDULED(concurrent)
{
  paxos::Server<int, int> server_1;
  paxos::Server<int, int> server_2;
  paxos::Server<int, int> server_3;
  auto peer_1_2 = new InstrumentedPeer<int, int>(server_2);
  auto peer_1_3 = new InstrumentedPeer<int, int>(server_3);
  typedef paxos::Client<int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(elle::make_unique<Peer<int, int>>(server_1));
  peers_1.push_back(std::unique_ptr<paxos::Client<int, int>::Peer>(peer_1_2));
  peers_1.push_back(std::unique_ptr<paxos::Client<int, int>::Peer>(peer_1_3));
  paxos::Client<int, int> client_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(elle::make_unique<Peer<int, int>>(server_1));
  peers_2.push_back(elle::make_unique<Peer<int, int>>(server_2));
  peers_2.push_back(elle::make_unique<Peer<int, int>>(server_3));
  paxos::Client<int, int> client_2(2, std::move(peers_2));
  peer_1_2->propose_barrier.open();
  peer_1_2->accept_barrier.open();
  peer_1_3->propose_barrier.open();
  reactor::Thread::unique_ptr t1(
    new reactor::Thread(
      "1",
      [&] { BOOST_CHECK_EQUAL(client_1.choose(42), 42); }));
  reactor::wait(
    reactor::Waitables({&peer_1_2->accept_signal, &peer_1_3->accept_signal}));
  BOOST_CHECK_EQUAL(client_2.choose(43), 42);
  peer_1_3->accept_barrier.open();
  reactor::wait(*t1);
}

ELLE_TEST_SCHEDULED(conflict)
{
  paxos::Server<int, int> server_1;
  paxos::Server<int, int> server_2;
  paxos::Server<int, int> server_3;
  auto peer_1_2 = new InstrumentedPeer<int, int>(server_2);
  auto peer_1_3 = new InstrumentedPeer<int, int>(server_3);
  typedef paxos::Client<int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(elle::make_unique<Peer<int, int>>(server_1));
  peers_1.push_back(std::unique_ptr<paxos::Client<int, int>::Peer>(peer_1_2));
  peers_1.push_back(std::unique_ptr<paxos::Client<int, int>::Peer>(peer_1_3));
  paxos::Client<int, int> client_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(elle::make_unique<UnavailablePeer<int, int>>());
  peers_2.push_back(elle::make_unique<Peer<int, int>>(server_2));
  peers_2.push_back(elle::make_unique<Peer<int, int>>(server_3));
  paxos::Client<int, int> client_2(2, std::move(peers_2));
  peer_1_2->propose_barrier.open();
  peer_1_3->propose_barrier.open();
  reactor::Thread::unique_ptr t1(
    new reactor::Thread(
      "1",
      [&] { BOOST_CHECK_EQUAL(client_1.choose(43), 42); }));
  reactor::wait(
    reactor::Waitables({&peer_1_2->accept_signal, &peer_1_3->accept_signal}));
  BOOST_CHECK(!client_2.choose(42));
  peer_1_2->accept_barrier.open();
  peer_1_3->accept_barrier.open();
  reactor::wait(*t1);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(all_is_well), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(two_of_three), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(one_of_three), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(already_chosen), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(concurrent), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(conflict), 0, valgrind(1));
}
