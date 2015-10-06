#include <elle/test.hh>

#include <reactor/Barrier.hh>
#include <reactor/scheduler.hh>
#include <reactor/signal.hh>

#include <athena/paxos/Paxos.hh>

ELLE_LOG_COMPONENT("athena.paxos.test");

namespace paxos = athena::paxos;

template <typename T, typename ServerId>
class Peer
  : public paxos::Paxos<T, ServerId>::Peer
{
public:
  virtual
  boost::optional<typename paxos::Paxos<T, ServerId>::Accepted>
  propose(ServerId const& sender, int round) override
  {
    return this->_paxos->propose(sender, round);
  }

  virtual
  typename paxos::Paxos<T, ServerId>::Proposal
  accept(ServerId const& sender, int round, T const& value) override
  {
    return this->_paxos->accept(sender, round, value);
  }

  ELLE_ATTRIBUTE_RW((paxos::Paxos<T, ServerId>*), paxos);
};

ELLE_TEST_SCHEDULED(all_is_well)
{
  auto peer_1_2 = new Peer<int, int>();
  auto peer_1_3 = new Peer<int, int>();
  auto peer_2_1 = new Peer<int, int>();
  auto peer_2_3 = new Peer<int, int>();
  auto peer_3_1 = new Peer<int, int>();
  auto peer_3_2 = new Peer<int, int>();
  typedef paxos::Paxos<int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_2));
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_3));
  paxos::Paxos<int, int> paxos_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_1));
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_3));
  paxos::Paxos<int, int> paxos_2(2, std::move(peers_2));
  Peers peers_3;
  peers_3.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_3_1));
  peers_3.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_3_2));
  paxos::Paxos<int, int> paxos_3(3, std::move(peers_3));
  peer_1_2->paxos(&paxos_2);
  peer_1_3->paxos(&paxos_3);
  peer_2_1->paxos(&paxos_1);
  peer_2_3->paxos(&paxos_3);
  peer_3_1->paxos(&paxos_1);
  peer_3_2->paxos(&paxos_2);
  paxos_1.choose(42);
  BOOST_CHECK_EQUAL(paxos_2.choose(), 42);
  BOOST_CHECK_EQUAL(paxos_3.choose(), 42);
}

template <typename T, typename ServerId>
class UnavailablePeer
  : public paxos::Paxos<T, ServerId>::Peer
{
  virtual
  boost::optional<typename paxos::Paxos<T, ServerId>::Accepted>
  propose(ServerId const& sender, int round) override
  {
    throw typename paxos::Paxos<T, ServerId>::Peer::Unavailable();
  }

  virtual
  typename paxos::Paxos<T, ServerId>::Proposal
  accept(ServerId const& sender, int round, T const& value) override
  {
    throw typename paxos::Paxos<T, ServerId>::Peer::Unavailable();
  }
};

ELLE_TEST_SCHEDULED(two_of_three)
{
  auto peer_1_2 = new Peer<int, int>();
  auto peer_1_3 = new UnavailablePeer<int, int>();
  auto peer_2_1 = new Peer<int, int>();
  auto peer_2_3 = new UnavailablePeer<int, int>();
  typedef paxos::Paxos<int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_2));
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_3));
  paxos::Paxos<int, int> paxos_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_1));
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_3));
  paxos::Paxos<int, int> paxos_2(2, std::move(peers_2));
  peer_1_2->paxos(&paxos_2);
  peer_2_1->paxos(&paxos_1);
  paxos_1.choose(42);
  BOOST_CHECK_EQUAL(paxos_2.choose(), 42);
}

ELLE_TEST_SCHEDULED(one_of_three)
{
  auto peer_1_2 = new UnavailablePeer<int, int>();
  auto peer_1_3 = new UnavailablePeer<int, int>();
  typedef paxos::Paxos<int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_2));
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_3));
  paxos::Paxos<int, int> paxos_1(1, std::move(peers_1));
  BOOST_CHECK_THROW(paxos_1.choose(42), elle::Error);
}

ELLE_TEST_SCHEDULED(already_chosen)
{
  auto peer_1_2 = new Peer<int, int>();
  auto peer_1_3 = new Peer<int, int>();
  auto peer_2_1 = new Peer<int, int>();
  auto peer_2_3 = new Peer<int, int>();
  auto peer_3_1 = new Peer<int, int>();
  auto peer_3_2 = new Peer<int, int>();
  typedef paxos::Paxos<int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_2));
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_3));
  paxos::Paxos<int, int> paxos_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_1));
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_3));
  paxos::Paxos<int, int> paxos_2(2, std::move(peers_2));
  Peers peers_3;
  peers_3.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_3_1));
  peers_3.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_3_2));
  paxos::Paxos<int, int> paxos_3(3, std::move(peers_3));
  peer_1_2->paxos(&paxos_2);
  peer_1_3->paxos(&paxos_3);
  peer_2_1->paxos(&paxos_1);
  peer_2_3->paxos(&paxos_3);
  peer_3_1->paxos(&paxos_1);
  peer_3_2->paxos(&paxos_2);
  paxos_1.choose(42);
  BOOST_CHECK_EQUAL(paxos_2.choose(43), 42);
  BOOST_CHECK_EQUAL(paxos_3.choose(44), 42);
}

template <typename T, typename ServerId>
class InstrumentedPeer
  : public Peer<T, ServerId>
{
public:
  InstrumentedPeer()
    : fail(false)
  {}

  virtual
  boost::optional<typename paxos::Paxos<T, ServerId>::Accepted>
  propose(ServerId const& sender, int round) override
  {
    if (fail)
      throw typename Peer<T, ServerId>::Unavailable();
    this->propose_signal.signal();
    reactor::wait(this->propose_barrier);
    return Peer<T, ServerId>::propose(sender, round);
  }

  virtual
  typename paxos::Paxos<T, ServerId>::Proposal
  accept(ServerId const& sender, int round, T const& value) override
  {
    if (fail)
      throw typename Peer<T, ServerId>::Unavailable();
    this->accept_signal.signal();
    reactor::wait(this->accept_barrier);
    return Peer<T, ServerId>::accept(sender, round, value);
  }

  bool fail;
  reactor::Barrier propose_barrier, accept_barrier;
  reactor::Signal propose_signal, accept_signal;
};

ELLE_TEST_SCHEDULED(concurrent)
{
  auto peer_1_2 = new InstrumentedPeer<int, int>();
  auto peer_1_3 = new InstrumentedPeer<int, int>();
  auto peer_2_1 = new Peer<int, int>();
  auto peer_2_3 = new Peer<int, int>();
  auto peer_3_1 = new Peer<int, int>();
  auto peer_3_2 = new Peer<int, int>();
  typedef paxos::Paxos<int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_2));
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_3));
  paxos::Paxos<int, int> paxos_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_1));
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_3));
  paxos::Paxos<int, int> paxos_2(2, std::move(peers_2));
  Peers peers_3;
  peers_3.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_3_1));
  peers_3.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_3_2));
  paxos::Paxos<int, int> paxos_3(3, std::move(peers_3));
  peer_1_2->paxos(&paxos_2);
  peer_1_3->paxos(&paxos_3);
  peer_2_1->paxos(&paxos_1);
  peer_2_3->paxos(&paxos_3);
  peer_3_1->paxos(&paxos_1);
  peer_3_2->paxos(&paxos_2);
  peer_1_2->propose_barrier.open();
  peer_1_2->accept_barrier.open();
  peer_1_3->propose_barrier.open();
  reactor::Thread::unique_ptr t1(
    new reactor::Thread(
      "1",
      [&] { BOOST_CHECK_EQUAL(paxos_1.choose(42), 42); }));
  reactor::wait(
    reactor::Waitables({&peer_1_2->accept_signal, &peer_1_3->accept_signal}));
  BOOST_CHECK_EQUAL(paxos_3.choose(43), 42);
  BOOST_CHECK_EQUAL(paxos_2.choose(), 42);
  peer_1_3->accept_barrier.open();
  reactor::wait(*t1);
}

ELLE_TEST_SCHEDULED(conflict)
{
  auto peer_1_2 = new InstrumentedPeer<int, int>();
  auto peer_1_3 = new InstrumentedPeer<int, int>();
  auto peer_2_1 = new Peer<int, int>();
  auto peer_2_3 = new Peer<int, int>();
  auto peer_3_1 = new UnavailablePeer<int, int>();
  auto peer_3_2 = new Peer<int, int>();
  typedef paxos::Paxos<int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_2));
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_3));
  paxos::Paxos<int, int> paxos_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_1));
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_3));
  paxos::Paxos<int, int> paxos_2(2, std::move(peers_2));
  Peers peers_3;
  peers_3.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_3_1));
  peers_3.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_3_2));
  paxos::Paxos<int, int> paxos_3(3, std::move(peers_3));
  peer_1_2->paxos(&paxos_2);
  peer_1_3->paxos(&paxos_3);
  peer_2_1->paxos(&paxos_1);
  peer_2_3->paxos(&paxos_3);
  peer_3_2->paxos(&paxos_2);
  peer_1_2->propose_barrier.open();
  peer_1_3->propose_barrier.open();
  reactor::Thread::unique_ptr t1(
    new reactor::Thread(
      "1",
      [&] { BOOST_CHECK_EQUAL(paxos_1.choose(43), 42); }));
  reactor::wait(
    reactor::Waitables({&peer_1_2->accept_signal, &peer_1_3->accept_signal}));
  BOOST_CHECK_EQUAL(paxos_3.choose(42), 42);
  peer_1_2->accept_barrier.open();
  peer_1_3->accept_barrier.open();
  reactor::wait(*t1);
  BOOST_CHECK_EQUAL(paxos_2.choose(), 42);
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
