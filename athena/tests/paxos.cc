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

template <typename T, typename Version, typename ServerId>
class Peer
  : public paxos::Client<T, Version, ServerId>::Peer
{
public:
  typedef paxos::Client<T, Version, ServerId> Client;

  Peer(paxos::Server<T, Version, ServerId>& paxos)
    : _paxos(paxos)
  {}

  virtual
  boost::optional<typename Client::Accepted>
  propose(
    typename paxos::Server<T, Version, ServerId>::Quorum const& q,
    typename Client::Proposal const& p) override
  {
    return this->_paxos.propose(q, p);
  }

  virtual
  typename Client::Proposal
  accept(
    typename paxos::Server<T, Version, ServerId>::Quorum const& q,
    typename Client::Proposal const& p,
    elle::Option<T, typename Client::Quorum> const& value) override
  {
    return this->_paxos.accept(q, p, value);
  }

  ELLE_ATTRIBUTE_RW((paxos::Server<T, Version, ServerId>&), paxos);
};

ELLE_TEST_SCHEDULED(all_is_well)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  typedef paxos::Client<int, int, int>::Peers Peers;
  Peers peers;
  peers.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers.push_back(elle::make_unique<Peer<int, int, int>>(server_2));
  peers.push_back(elle::make_unique<Peer<int, int, int>>(server_3));
  paxos::Client<int, int, int> client(1, std::move(peers));
  try
  {
    BOOST_CHECK(!client.choose({11, 12, 13}, 42));
  }
  catch (paxos::Server<int, int, int>::WrongQuorum const& q)
  {
    BOOST_CHECK_EQUAL(q.effective(), q.expected());
    throw;
  }
}

template <typename T, typename Version, typename ServerId>
class UnavailablePeer
  : public paxos::Client<T, Version, ServerId>::Peer
{
public:
  typedef paxos::Client<T, Version, ServerId> Client;

  virtual
  boost::optional<typename Client::Accepted>
  propose(
    typename Client::Quorum const& q,
    typename Client::Proposal const& p) override
  {
    throw typename Client::Peer::Unavailable();
  }

  virtual
  typename Client::Proposal
  accept(typename Client::Quorum const& q,
         typename Client::Proposal const& p,
         elle::Option<T, typename Client::Quorum> const& value) override
  {
    throw typename Client::Peer::Unavailable();
  }
};

ELLE_TEST_SCHEDULED(two_of_three)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  typedef paxos::Client<int, int, int>::Peers Peers;
  Peers peers;
  peers.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers.push_back(elle::make_unique<Peer<int, int, int>>(server_2));
  peers.push_back(elle::make_unique<UnavailablePeer<int, int, int>>());
  paxos::Client<int, int, int> client(1, std::move(peers));
  BOOST_CHECK(!client.choose({11, 12, 13}, 42));
}

ELLE_TEST_SCHEDULED(one_of_three)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  typedef paxos::Client<int, int, int>::Peers Peers;
  Peers peers;
  peers.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers.push_back(elle::make_unique<UnavailablePeer<int, int, int>>());
  peers.push_back(elle::make_unique<UnavailablePeer<int, int, int>>());
  paxos::Client<int, int, int> client(1, std::move(peers));
  BOOST_CHECK_THROW(client.choose({11, 12, 13}, 42), elle::Error);
}

ELLE_TEST_SCHEDULED(already_chosen)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  typedef paxos::Client<int, int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers_1.push_back(elle::make_unique<Peer<int, int, int>>(server_2));
  peers_1.push_back(elle::make_unique<Peer<int, int, int>>(server_3));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_2));
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_3));
  paxos::Client<int, int, int> client_2(1, std::move(peers_2));
  BOOST_CHECK(!client_1.choose({11, 12, 13}, 42));
  auto chosen = client_2.choose({11, 12, 13}, 43);
  BOOST_CHECK(chosen);
  BOOST_CHECK(chosen->is<int>());
  BOOST_CHECK_EQUAL(chosen->get<int>(), 42);
}

template <typename T, typename Version, typename ServerId>
class InstrumentedPeer
  : public Peer<T, Version, ServerId>
{
public:
  typedef paxos::Client<T, Version, ServerId> Client;

  InstrumentedPeer(paxos::Server<T, Version, ServerId>& paxos)
    : Peer<T, Version, ServerId>(paxos)
    , fail(false)
  {}

  ~InstrumentedPeer() noexcept(true)
  {}

  virtual
  boost::optional<typename Client::Accepted>
  propose(
    typename Client::Quorum const& q,
    typename Client::Proposal const& p) override
  {
    if (fail)
      throw typename Peer<T, Version, ServerId>::Unavailable();
    this->propose_signal.signal();
    reactor::wait(this->propose_barrier);
    return Peer<T, Version, ServerId>::propose(q, p);
  }

  virtual
  typename Client::Proposal
  accept(typename Client::Quorum const& q,
         typename Client::Proposal const& p,
         elle::Option<T, typename Client::Quorum> const& value) override
  {
    if (fail)
      throw typename Peer<T, Version, ServerId>::Unavailable();
    this->accept_signal.signal();
    reactor::wait(this->accept_barrier);
    return Peer<T, Version, ServerId>::accept(q, p, value);
  }

  bool fail;
  reactor::Barrier propose_barrier, accept_barrier;
  reactor::Signal propose_signal, accept_signal;
};

ELLE_TEST_SCHEDULED(concurrent)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  auto peer_1_2 = new InstrumentedPeer<int, int, int>(server_2);
  auto peer_1_3 = new InstrumentedPeer<int, int, int>(server_3);
  typedef paxos::Client<int, int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers_1.push_back(std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_2));
  peers_1.push_back(std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_3));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_2));
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_3));
  paxos::Client<int, int, int> client_2(2, std::move(peers_2));
  peer_1_2->propose_barrier.open();
  peer_1_2->accept_barrier.open();
  peer_1_3->propose_barrier.open();
  reactor::Thread::unique_ptr t1(
    new reactor::Thread(
      "1",
      [&]
      {
        auto chosen = client_1.choose({11, 12, 13}, 42);
        BOOST_CHECK_EQUAL(chosen->get<int>(), 42);
      }));
  reactor::wait(
    reactor::Waitables({&peer_1_2->accept_signal, &peer_1_3->accept_signal}));
  auto chosen = client_2.choose({11, 12, 13}, 43);
  BOOST_CHECK_EQUAL(chosen->get<int>(), 42);
  peer_1_3->accept_barrier.open();
  reactor::wait(*t1);
}

ELLE_TEST_SCHEDULED(conflict)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  auto peer_1_2 = new InstrumentedPeer<int, int, int>(server_2);
  auto peer_1_3 = new InstrumentedPeer<int, int, int>(server_3);
  typedef paxos::Client<int, int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers_1.push_back(
    std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_2));
  peers_1.push_back(
    std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_3));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(elle::make_unique<UnavailablePeer<int, int, int>>());
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_2));
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_3));
  paxos::Client<int, int, int> client_2(2, std::move(peers_2));
  peer_1_2->propose_barrier.open();
  peer_1_3->propose_barrier.open();
  reactor::Thread::unique_ptr t1(
    new reactor::Thread(
      "1",
      [&]
      {
        auto chosen = client_1.choose({11, 12, 13}, 43);
        BOOST_CHECK_EQUAL(chosen->get<int>(), 42);
      }));
  reactor::wait(
    reactor::Waitables({&peer_1_2->accept_signal, &peer_1_3->accept_signal}));
  BOOST_CHECK(!client_2.choose({11, 12, 13}, 42));
  peer_1_2->accept_barrier.open();
  peer_1_3->accept_barrier.open();
  reactor::wait(*t1);
}

// Check a newer version overrides a previous result
ELLE_TEST_SCHEDULED(versions)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  typedef paxos::Client<int, int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers_1.push_back(elle::make_unique<Peer<int, int, int>>(server_2));
  peers_1.push_back(elle::make_unique<Peer<int, int, int>>(server_3));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_2));
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_3));
  paxos::Client<int, int, int> client_2(1, std::move(peers_2));
  BOOST_CHECK(!client_1.choose({11, 12, 13}, 1, 1));
  BOOST_CHECK(!client_2.choose({11, 12, 13}, 2, 2));
}

// Check a newer version, even partially agreed on, is taken in account
ELLE_TEST_SCHEDULED(versions_partial)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  typedef paxos::Client<int, int, int>::Peers Peers;
  Peers peers_1;
  auto peer_1_1 = new InstrumentedPeer<int, int, int>(server_1);
  peers_1.push_back(
    std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_1));
  peer_1_1->propose_barrier.open();
  peer_1_1->accept_barrier.open();
  auto peer_1_2 = new InstrumentedPeer<int, int, int>(server_2);
  peer_1_2->propose_barrier.open();
  peers_1.push_back(
    std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_2));
  auto peer_1_3 = new InstrumentedPeer<int, int, int>(server_3);
  peer_1_3->propose_barrier.open();
  peers_1.push_back(
    std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_3));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_2));
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_3));
  paxos::Client<int, int, int> client_2(2, std::move(peers_2));
  reactor::Thread t("client_1",
                    [&]
                    {
                      BOOST_CHECK(!client_1.choose({11, 12, 13}, 2, 2));
                    });
  reactor::wait(peer_1_1->accept_signal);
  auto chosen = client_2.choose({11, 12, 13}, 1, 1);
  BOOST_CHECK_EQUAL(chosen->get<int>(), 2);
  peer_1_2->accept_barrier.open();
  peer_1_3->accept_barrier.open();
  reactor::wait(t);
}

// Check a failed newer version doesn't block older ones
ELLE_TEST_SCHEDULED(versions_aborted)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  typedef paxos::Client<int, int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(elle::make_unique<Peer<int, int, int>>(server_1));
  peers_1.push_back(elle::make_unique<UnavailablePeer<int, int, int>>());
  peers_1.push_back(elle::make_unique<UnavailablePeer<int, int, int>>());
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(elle::make_unique<UnavailablePeer<int, int, int>>());
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_2));
  peers_2.push_back(elle::make_unique<Peer<int, int, int>>(server_3));
  paxos::Client<int, int, int> client_2(2, std::move(peers_2));
  BOOST_CHECK_THROW(client_1.choose({11, 12, 13}, 2, 2), elle::Error);
  BOOST_CHECK(!client_2.choose({11, 12, 13}, 1, 1));
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
  suite.add(BOOST_TEST_CASE(versions), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(versions_partial), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(versions_aborted), 0, valgrind(1));
}
