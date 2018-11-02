#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/irange.hpp>

#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

#include <elle/reactor/Barrier.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/signal.hh>

#include <elle/athena/paxos/Client.hh>
#include <elle/athena/paxos/Server.hh>

ELLE_LOG_COMPONENT("elle.athena.paxos.test");

namespace paxos = elle::athena::paxos;

using boost::adaptors::sliced;
using boost::irange;

namespace std
{
  std::ostream&
  operator <<(std::ostream& output, boost::optional<int> const& v);

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
  using Client = paxos::Client<T, Version, ServerId>;

  Peer(ServerId id,
       paxos::Server<T, Version, ServerId>& paxos)
    : paxos::Client<T, Version, ServerId>::Peer(id)
    , _paxos(paxos)
  {}


  typename Client::Response
  propose(
    typename paxos::Server<T, Version, ServerId>::Quorum const& q,
    typename Client::Proposal const& p) override
  {
    return this->_paxos.propose(q, p);
  }


  typename Client::Proposal
  accept(
    typename paxos::Server<T, Version, ServerId>::Quorum const& q,
    typename Client::Proposal const& p,
    elle::Option<T, typename Client::Quorum> const& value) override
  {
    return this->_paxos.accept(q, p, value);
  }


  void
  confirm(
    typename paxos::Server<T, Version, ServerId>::Quorum const& q,
    typename Client::Proposal const& p) override
  {
    return this->_paxos.confirm(q, p);
  }


  boost::optional<typename Client::Accepted>
  get(
    typename paxos::Server<T, Version, ServerId>::Quorum const& q) override
  {
    return this->_paxos.get(q);
  }

  ELLE_ATTRIBUTE_RW((paxos::Server<T, Version, ServerId>&), paxos);
};

using Server = paxos::Server<int, int, int>;
using Client = paxos::Client<int, int, int>;
using Peers = Client::Peers;

class YAInstrumentedPeer
  : public Peer<int, int, int>
{
public:
  using Super = Peer<int, int, int>;
  using Client = paxos::Client<int, int, int>;

  using Super::Super;

  typename Client::Response
  propose(typename Client::Quorum const& q,
          typename Client::Proposal const& p) override
  {
    this->_proposing(p);
    auto res = Super::propose(q, p);
    this->_proposed(p);
    return res;
  }

  typename Client::Proposal
  accept(typename Client::Quorum const& q,
         typename Client::Proposal const& p,
         elle::Option<int, typename Client::Quorum> const& value) override
  {
    this->_accepting(p);
    auto res = Super::accept(q, p, value);
    this->_accepted(p);
    return res;
  }


  void
  confirm(typename Client::Quorum const& q,
          typename Client::Proposal const& p) override
  {
    this->_confirming(p);
    Super::confirm(q, p);
    this->_confirmed(p);
  }

  ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (Server::Proposal const&)>,
                    proposing);
  ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (Server::Proposal const&)>,
                    proposed);
  ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (Server::Proposal const&)>,
                    accepting);
  ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (Server::Proposal const&)>,
                    accepted);
  ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (Server::Proposal const&)>,
                    confirming);
  ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (Server::Proposal const&)>,
                    confirmed);
};

template <typename Peer = YAInstrumentedPeer, typename Servers>
Client
make_client(int id, Servers&& servers)
{
  auto peers = Peers();
  for (auto& s: servers)
    peers.emplace_back(new Peer(s.id(), s));
  return Client(id, std::move(peers));
}

ELLE_TEST_SCHEDULED(all_is_well)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  using Peers = paxos::Client<int, int, int>::Peers;
  auto peers = Peers{};
  peers.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
  peers.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
  peers.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
  paxos::Client<int, int, int> client(1, std::move(peers));
  try
  {
    BOOST_CHECK(!client.choose(42));
  }
  catch (paxos::Server<int, int, int>::WrongQuorum const& q)
  {
    BOOST_CHECK_EQUAL(q.effective(), q.expected());
    throw;
  }
  BOOST_CHECK_EQUAL(client.get(), 42);
}

template <typename T, typename Version, typename ServerId>
class UnavailablePeer
  : public paxos::Client<T, Version, ServerId>::Peer
{
public:
  using Client = paxos::Client<T, Version, ServerId>;

  UnavailablePeer(ServerId id)
    : paxos::Client<T, Version, ServerId>::Peer(std::move(id))
  {}


  typename Client::Response
  propose(typename Client::Quorum const& q,
          typename Client::Proposal const& p) override
  {
    throw paxos::Unavailable();
  }


  typename Client::Proposal
  accept(typename Client::Quorum const& q,
         typename Client::Proposal const& p,
         elle::Option<T, typename Client::Quorum> const& value) override
  {
    throw paxos::Unavailable();
  }


  void
  confirm(typename Client::Quorum const& q,
          typename Client::Proposal const& p) override
  {
    throw paxos::Unavailable();
  }


  boost::optional<typename Client::Accepted>
  get(typename Client::Quorum const& q) override
  {
    throw paxos::Unavailable();
  }
};

ELLE_TEST_SCHEDULED(two_of_three)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  using Peers = paxos::Client<int, int, int>::Peers;
  auto peers = Peers{};
  peers.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
  peers.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
  peers.emplace_back(std::make_unique<UnavailablePeer<int, int, int>>(13));
  paxos::Client<int, int, int> client(1, std::move(peers));
  BOOST_CHECK(!client.choose(42));
  BOOST_CHECK_EQUAL(client.get(), 42);
}

ELLE_TEST_SCHEDULED(one_of_three)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  using Peers = paxos::Client<int, int, int>::Peers;
  auto peers = Peers{};
  peers.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
  peers.emplace_back(std::make_unique<UnavailablePeer<int, int, int>>(12));
  peers.emplace_back(std::make_unique<UnavailablePeer<int, int, int>>(13));
  paxos::Client<int, int, int> client(1, std::move(peers));
  BOOST_CHECK_THROW(client.choose(42), elle::athena::paxos::TooFewPeers);
  BOOST_CHECK_THROW(client.get(), elle::athena::paxos::TooFewPeers);
}

ELLE_TEST_SCHEDULED(already_chosen)
{
  std::vector<Server> servers
  {
    {11, {11, 12, 13}},
    {12, {11, 12, 13}},
    {13, {11, 12, 13}},
  };
  ELLE_LOG("chose 42 from client 1")
  {
    auto client_1 = make_client(1, servers);
    BOOST_CHECK(!client_1.choose(42));
  }
  ELLE_LOG("chose 43 from client 2")
  {
    auto client_2 = make_client(2, servers);
    auto chosen = client_2.choose(43);
    BOOST_REQUIRE(chosen);
    BOOST_CHECK(chosen->is<int>());
    BOOST_CHECK_EQUAL(chosen->get<int>(), 42);
  }
}

template <typename T, typename Version, typename ServerId>
class InstrumentedPeer
  : public Peer<T, Version, ServerId>
{
public:
  using Super = Peer<T, Version, ServerId>;
  using Client = paxos::Client<T, Version, ServerId>;

  InstrumentedPeer(ServerId id, paxos::Server<T, Version, ServerId>& paxos,
                   bool go_through = false)
    : Super{id, paxos}
    , fail(false)
    , propose_barrier("propose barrier")
    , accept_barrier("accept barrier")
    , confirm_barrier("confirm barrier")
  {
    if (go_through)
    {
      this->propose_barrier.open();
      this->accept_barrier.open();
      this->confirm_barrier.open();
    }
  }

  ~InstrumentedPeer() noexcept(true) override
  {}


  typename Client::Response
  propose(
    typename Client::Quorum const& q,
    typename Client::Proposal const& p) override
  {
    if (fail)
      throw paxos::Unavailable();
    else
    {
      this->propose_signal.signal();
      elle::reactor::wait(this->propose_barrier);
      return Super::propose(q, p);
    }
  }


  typename Client::Proposal
  accept(typename Client::Quorum const& q,
         typename Client::Proposal const& p,
         elle::Option<T, typename Client::Quorum> const& value) override
  {
    if (fail)
      throw paxos::Unavailable();
    else
    {
      this->accept_signal.signal();
      elle::reactor::wait(this->accept_barrier);
      return Super::accept(q, p, value);
    }
  }


  void
  confirm(typename Client::Quorum const& q,
          typename Client::Proposal const& p) override
  {
    if (fail)
      throw paxos::Unavailable();
    else
    {
      this->confirm_signal.signal();
      elle::reactor::wait(this->confirm_barrier);
      return Super::confirm(q, p);
    }
  }

  bool fail;
  elle::reactor::Barrier propose_barrier, accept_barrier, confirm_barrier;
  elle::reactor::Signal propose_signal, accept_signal, confirm_signal;
};

ELLE_TEST_SCHEDULED(concurrent)
{
  std::vector<Server> servers
  {
    {11, {11, 12, 13}},
    {12, {11, 12, 13}},
    {13, {11, 12, 13}},
  };
  auto peer_1_2 = new InstrumentedPeer<int, int, int>(12, servers[1]);
  auto peer_1_3 = new InstrumentedPeer<int, int, int>(13, servers[2]);
  using Peers = paxos::Client<int, int, int>::Peers;
  auto peers_1 = Peers{};
  peers_1.emplace_back(std::make_unique<Peer<int, int, int>>(11, servers[0]));
  peers_1.emplace_back(std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_2));
  peers_1.emplace_back(std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_3));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  client_1.conflict_backoff(false);
  auto client_2 = make_client(2, servers);
  client_2.conflict_backoff(false);
  peer_1_2->propose_barrier.open();
  peer_1_2->accept_barrier.open();
  peer_1_3->propose_barrier.open();
  elle::reactor::Thread::unique_ptr t1(
    new elle::reactor::Thread(
      "1",
      [&]
      {
        auto chosen = client_1.choose(42);
        BOOST_CHECK_EQUAL(chosen->get<int>(), 42);
      }));
  elle::reactor::wait(
    elle::reactor::Waitables({&peer_1_2->accept_signal, &peer_1_3->accept_signal}));
  auto chosen = client_2.choose(43);
  BOOST_REQUIRE(chosen);
  BOOST_CHECK_EQUAL(chosen->get<int>(), 42);
  peer_1_3->accept_barrier.open();
  elle::reactor::wait(*t1);
}

ELLE_TEST_SCHEDULED(conflict)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  auto peer_1_2 = new InstrumentedPeer<int, int, int>(12, server_2);
  auto peer_1_3 = new InstrumentedPeer<int, int, int>(13, server_3);
  using Peers = paxos::Client<int, int, int>::Peers;
  auto peers_1 = Peers{};
  peers_1.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
  peers_1.emplace_back(
    std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_2));
  peers_1.emplace_back(
    std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_3));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  client_1.conflict_backoff(false);
  auto peers_2 = Peers{};
  peers_2.emplace_back(std::make_unique<UnavailablePeer<int, int, int>>(11));
  peers_2.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
  peers_2.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
  paxos::Client<int, int, int> client_2(2, std::move(peers_2));
  client_2.conflict_backoff(false);
  peer_1_2->propose_barrier.open();
  peer_1_3->propose_barrier.open();
  elle::reactor::Thread::unique_ptr t1(
    new elle::reactor::Thread(
      "1",
      [&]
      {
        auto chosen = client_1.choose(43);
        BOOST_CHECK_EQUAL(chosen->get<int>(), 42);
      }));
  elle::reactor::wait(
    elle::reactor::Waitables({&peer_1_2->accept_signal, &peer_1_3->accept_signal}));
  BOOST_CHECK(!client_2.choose(42));
  peer_1_2->accept_barrier.open();
  peer_1_3->accept_barrier.open();
  elle::reactor::wait(*t1);
}

// Check a newer version overrides a previous result
ELLE_TEST_SCHEDULED(versions)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  using Peers = paxos::Client<int, int, int>::Peers;
  auto peers_1 = Peers{};
  peers_1.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
  peers_1.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
  peers_1.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  auto peers_2 = Peers{};
  peers_2.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
  peers_2.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
  peers_2.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
  paxos::Client<int, int, int> client_2(1, std::move(peers_2));
  ELLE_LOG("choose 1 for version 1")
    BOOST_CHECK(!client_1.choose(1, 1));
  ELLE_LOG("choose 2 for version 2")
    BOOST_CHECK(!client_2.choose(2, 2));
}

// Check a newer version, even partially agreed on, is taken in account
ELLE_TEST_SCHEDULED(versions_partial)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  using Peers = paxos::Client<int, int, int>::Peers;
  auto peers_1 = Peers{};
  auto peer_1_1 = new InstrumentedPeer<int, int, int>(11, server_1);
  peers_1.emplace_back(
    std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_1));
  peer_1_1->propose_barrier.open();
  peer_1_1->accept_barrier.open();
  auto peer_1_2 = new InstrumentedPeer<int, int, int>(12, server_2);
  peer_1_2->propose_barrier.open();
  peers_1.emplace_back(
    std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_2));
  auto peer_1_3 = new InstrumentedPeer<int, int, int>(13, server_3);
  peer_1_3->propose_barrier.open();
  peers_1.emplace_back(
    std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_3));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  client_1.conflict_backoff(false);
  ELLE_LOG("select 1 for version 1 (client 2)")
  {
    auto peers_2 = Peers{};
    peers_2.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
    peers_2.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
    peers_2.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
    paxos::Client<int, int, int> client_2(2, std::move(peers_2));
    client_2.conflict_backoff(false);
    BOOST_CHECK(!client_2.choose(1, 1));
  }
  //        11        12        13
  //   +---------+---------+---------+
  // 1 |    1    |    1    |    1    | -> 1
  //   |  1:1:2  |  1:1:2  |  1:1:2  |
  //   +---------+---------+---------+
  elle::reactor::Thread::unique_ptr t(
    new elle::reactor::Thread("client_1",
                        [&]
                        {
                          auto chosen = client_1.choose(2, 2);
                          BOOST_CHECK(chosen);
                          BOOST_CHECK_EQUAL(chosen->get<int>(), 2);
                        }));
  elle::reactor::wait(peer_1_1->accept_signal);
  //        11        12        13
  //   +---------+---------+---------+
  // 2 |    2    |         |         | -> ?
  //   |  2:1:1  |  2:1:1  |  2:1:1  |
  //   +---------+---------+---------+
  // 1 |    1    |    1    |    1    | -> 1
  //   |  1:1:2  |  1:1:2  |  1:1:2  |
  //   +---------+---------+---------+
  ELLE_LOG("select 1 for version 1 (client 3)")
  {
    auto peers_3 = Peers{};
    peers_3.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
    peers_3.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
    peers_3.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
    paxos::Client<int, int, int> client_3(3, std::move(peers_3));
    auto chosen = client_3.choose(1, 1);
    client_3.conflict_backoff(false);
    BOOST_CHECK_EQUAL(chosen->get<int>(), 2);
  }
  //        11        12        13
  //   +---------+---------+---------+
  // 2 |    2    |    2    |    2    | -> 2
  //   |  2:2:3  |  2:2:3  |  2:2:3  |
  //   +---------+---------+---------+
  // 1 |    1    |    1    |    1    | -> 1
  //   |  1:1:2  |  1:1:2  |  1:1:2  |
  //   +---------+---------+---------+
  peer_1_2->accept_barrier.open();
  peer_1_3->accept_barrier.open();
  elle::reactor::wait(*t);
  //        11        12        13
  //   +---------+---------+---------+
  // 2 |    2    |    2    |    2    | -> 2
  //   |  2:3:1  |  2:3:1  |  2:3:1  |
  //   +---------+---------+---------+
  // 1 |    1    |    1    |    1    | -> 1
  //   |  1:1:2  |  1:1:2  |  1:1:2  |
  //   +---------+---------+---------+
}

// Check a failed newer version doesn't block older ones
ELLE_TEST_SCHEDULED(versions_aborted)
{
  paxos::Server<int, int, int> server_1(11, {11, 12, 13});
  paxos::Server<int, int, int> server_2(12, {11, 12, 13});
  paxos::Server<int, int, int> server_3(13, {11, 12, 13});
  using Peers = paxos::Client<int, int, int>::Peers;
  auto peers_1 = Peers{};
  peers_1.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
  peers_1.emplace_back(std::make_unique<UnavailablePeer<int, int, int>>(12));
  peers_1.emplace_back(std::make_unique<UnavailablePeer<int, int, int>>(13));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  auto peers_2 = Peers{};
  peers_2.emplace_back(std::make_unique<UnavailablePeer<int, int, int>>(11));
  peers_2.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
  peers_2.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
  paxos::Client<int, int, int> client_2(2, std::move(peers_2));
  BOOST_CHECK_THROW(client_1.choose(2, 2), elle::Error);
  BOOST_CHECK(!client_2.choose(1, 1));
}

ELLE_TEST_SCHEDULED(propose_before_current_proposal_acceptation)
{
  using Server = paxos::Server<int, int, int>;
  using Client = paxos::Client<int, int, int>;
  using Peer = Peer<int, int, int>;
  using Peers = Client::Peers;
  Server server_1(11, {11, 12, 13});
  Server server_2(12, {11, 12, 13});
  Server server_3(13, {11, 12, 13});
  // Client 1.
  auto peers_1 = Peers{};
  peers_1.emplace_back(std::make_unique<Peer>(11, server_1));
  peers_1.emplace_back(std::make_unique<Peer>(12, server_2));
  auto peer_1_3 = new InstrumentedPeer<int, int, int>(13, server_3, true);
  peers_1.emplace_back(std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_1_3));
  paxos::Client<int, int, int> client_1(1, std::move(peers_1));
  // Client 2
  auto peers_2 = Peers{};
  peers_2.emplace_back(std::make_unique<Peer>(11, server_1));
  peers_2.emplace_back(std::make_unique<Peer>(12, server_2));
  auto peer_2_3 = new InstrumentedPeer<int, int, int>(13, server_3, true);
  peers_2.emplace_back(std::unique_ptr<paxos::Client<int, int, int>::Peer>(peer_2_3));
  paxos::Client<int, int, int> client_2(2, std::move(peers_2));
  // Do not confirm first proposal.
  peer_1_3->confirm_barrier.close();
  // Block acceptance of the seccond proposal.
  peer_2_3->accept_barrier.close();
  // Actions.
  elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
  {
    scope.run_background("client_1 choose",
                         [&]
                         {
                           // Client 1 choose 1:1.
                           client_1.choose(1, 1);
                         });
    scope.run_background("client_2 fetch & choose new version",
                         [&]
                         {
                           elle::reactor::wait(peer_1_3->confirm_signal);
                           // Client 2 choose 2:1.
                           auto v = client_2.get();
                           BOOST_CHECK(v);
                           BOOST_CHECK_EQUAL(*v, 1);
                           client_2.choose(2, 2);
                         });
    scope.run_background("confirm",
                         [&]
                         {
                           elle::reactor::wait(peer_1_3->confirm_signal);
                           elle::reactor::wait(peer_2_3->propose_signal);
                           peer_1_3->confirm_barrier.open();
                           // XXX: Find a better way.
                           elle::reactor::yield();
                           elle::reactor::yield();
                           elle::reactor::yield();
                           peer_2_3->accept_barrier.open();
                         });
    scope.wait();
  };
}

ELLE_TEST_SCHEDULED(elect_extend)
{
  using Server = paxos::Server<int, int, int>;
  using Client = paxos::Client<int, int, int>;
  using Peers = Client::Peers;
  Server server_1(11, {11});
  Server server_2(12, {11, 12});
  auto peers = Peers{};
  peers.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
  paxos::Client<int, int, int> client(1, std::move(peers));
  ELLE_LOG("choose 0 for version 0")
    BOOST_CHECK(!client.choose(0, 0));
  //        11
  //   +---------+
  // 0 |    0    | -> 1
  //   |  1:1:1  |
  //   +---------+
  ELLE_LOG("fail choosing 1 for version 0")
    BOOST_CHECK_EQUAL(client.choose(0, 1)->get<int>(), 0);
  ELLE_LOG("fail choosing quorum of 2 for version 0");
    BOOST_CHECK_EQUAL(
      client.choose(0, Client::Quorum{11, 12})->get<int>(), 0);
  ELLE_LOG("choose quorum of 2 for version 1")
    BOOST_CHECK(!client.choose(1, Client::Quorum{11, 12}));
  //        11
  //   +---------+
  // 1 | {11,12} | -> {11,12}
  //   |  1:1:1  |
  //   +---------+
  // 0 |    0    | -> 1
  //   |  0:1:1  |
  //   +---------+
  ELLE_LOG("fail choosing 1 for version 1")
    BOOST_CHECK_EQUAL(client.choose(1, 1)->get<Client::Quorum>(),
                      Client::Quorum({11, 12}));
  ELLE_LOG("fail choosing 2 for version 2 with partial quorum")
    BOOST_CHECK_THROW(client.choose(2, 2), Server::WrongQuorum);
  client.peers().emplace_back(
    std::make_unique<Peer<int, int, int>>(12, server_2));
  ELLE_LOG("choose 2 for version 2 with extended quorum")
    BOOST_CHECK(!client.choose(2, 2));
  //        11
  //   +---------+---------+
  // 2 |    2    |    2    | -> 2
  //   |  2:1:1  |  2:1:1  |
  //   +---------+---------+
  // 1 | {11,12} |           -> {11,12}
  //   |  1:1:1  |
  //   +---------+
  // 0 |    0    |           -> 1
  //   |  0:1:1  |
  //   +---------+
  ELLE_LOG("choose 3 for version 3 with extended quorum")
    BOOST_CHECK(!client.choose(3, 3));
  //        11
  //   +---------+---------+
  // 3 |    3    |    3    | -> 3
  //   |  3:1:1  |  3:1:1  |
  //   +---------+---------+
  // 2 |    2    |    2    | -> 2
  //   |  2:1:1  |  3:1:1  |
  //   +---------+---------+
  // 1 | {11,12} |           -> {11,12}
  //   |  1:1:1  |
  //   +---------+
  // 0 |    0    |           -> 1
  //   |  0:1:1  |
  //   +---------+
}

ELLE_TEST_SCHEDULED(elect_shrink)
{
  using Server = paxos::Server<int, int, int>;
  using Client = paxos::Client<int, int, int>;
  using Peer = Peer<int, int, int>;
  using Peers = Client::Peers;
  Server server_1(11, {11, 12});
  Server server_2(12, {11, 12});
  auto peers = Peers{};
  peers.emplace_back(std::make_unique<Peer>(11, server_1));
  peers.emplace_back(std::make_unique<Peer>(12, server_2));
  paxos::Client<int, int, int> client(1, std::move(peers));
  ELLE_LOG("choose initial value")
    BOOST_CHECK(!client.choose(0, 0));
  ELLE_LOG("choose quorum of 1")
    BOOST_CHECK(!client.choose(1, Client::Quorum{11}));
  ELLE_LOG("fail choosing with superfluous server")
    BOOST_CHECK_THROW(client.choose(2, 2), Server::WrongQuorum);
  ELLE_LOG("drop peer 2 and choose")
  {
    client.peers().pop_back();
    BOOST_CHECK(!client.choose(2, 2));
  }
}

ELLE_TEST_SCHEDULED(evict_down_lag_behind)
{
  using Server = paxos::Server<int, int, int>;
  using Client = paxos::Client<int, int, int>;
  using Peer = Peer<int, int, int>;
  using Peers = Client::Peers;
  Server server_1(11, {11, 12, 13});
  Server server_2(12, {11, 12, 13});
  Server server_3(13, {11, 12, 13});
  auto make_client = [&]
    {
      auto peers = Peers{};
      peers.emplace_back(std::make_unique<Peer>(11, server_1));
      peers.emplace_back(std::make_unique<Peer>(12, server_2));
      peers.emplace_back(std::make_unique<Peer>(13, server_3));
      return paxos::Client<int, int, int>(1, std::move(peers));
    };
  auto make_2client = [&]
    {
      auto peers = Peers{};
      peers.emplace_back(std::make_unique<Peer>(11, server_1));
      peers.emplace_back(std::make_unique<Peer>(12, server_2));
      return paxos::Client<int, int, int>(1, std::move(peers));
    };
  auto make_partial_client = [&]
    {
      auto peers = Peers{};
      peers.emplace_back(std::make_unique<Peer>(11, server_1));
      peers.emplace_back(std::make_unique<Peer>(12, server_2));
      peers.emplace_back(std::make_unique<UnavailablePeer<int, int, int>>(13));
      return paxos::Client<int, int, int>(1, std::move(peers));
    };
  ELLE_LOG("choose 1 for version 1")
    BOOST_CHECK(!make_client().choose(1, 1));
  ELLE_LOG("choose 2 for version 2")
    BOOST_CHECK(!make_client().choose(2, 2));
  //        11        12        13
  //   +---------+---------+---------+
  // 2 |    2    |    2    |    2    | -> 2
  //   |  2:1:1  |  2:1:1  |  2:1:1  |
  //   +---------+---------+---------+
  // 1 |    1    |    1    |    1    | -> 1
  //   |  1:1:1  |  1:1:1  |  1:1:1  |
  //   +---------+---------+---------+
  ELLE_LOG("choose quorum \\{11, 12\\} for version 3")
    BOOST_CHECK(!make_partial_client().choose(3, Client::Quorum{11, 12}));
  //        11        12        13
  //   +---------+---------+---------+
  // 3 | {11,12} | {11,12} |         | -> {11,12}
  //   |  3:1:1  |  3:1:1  |         |
  //   +---------+---------+---------+
  // 2 |    2    |    2    |    2    | -> 2
  //   |  2:1:1  |  2:1:1  |  2:1:1  |
  //   +---------+---------+---------+
  // 1 |    1    |    1    |    1    | -> 1
  //   |  1:1:1  |  1:1:1  |  1:1:1  |
  //   +---------+---------+---------+
  ELLE_LOG("choose 21 for version 2")
    // Both exception and response are deemed valid behavior
    try
    {
      auto res = make_2client().choose(2, 21);
      BOOST_CHECK(res);
      BOOST_CHECK(res->is<Client::Quorum>());
      BOOST_CHECK_EQUAL(res->get<Client::Quorum>(), (Client::Quorum{11, 12}));
    }
    catch (Server::WrongQuorum const& e)
    {
      BOOST_CHECK_EQUAL(e.effective(), (Client::Quorum{11, 12}));
      BOOST_CHECK_EQUAL(e.expected(), (Client::Quorum{11, 12, 13}));
    }
  ELLE_LOG("choose 21 for version 2")
  {
    auto chosen = make_partial_client().choose(2, 21);
    BOOST_CHECK(chosen);
    BOOST_CHECK_EQUAL(chosen->get<Client::Quorum>(),
                      (Client::Quorum{11, 12}));
    BOOST_CHECK_EQUAL(chosen.proposal().version, 3);
  }
  ELLE_LOG("choose 4 for version 4")
    BOOST_CHECK(!make_2client().choose(4, 4));
}

template <typename T, typename Version, typename ServerId>
class ProposeOnlyPeer
  : public Peer<T, Version, ServerId>
{
public:
  using Super = Peer<T, Version, ServerId>;
  using Client = paxos::Client<T, Version, ServerId>;

  ProposeOnlyPeer(ServerId id, paxos::Server<T, Version, ServerId>& paxos)
    : Super{id, paxos}
  {}

  typename Client::Proposal
  accept(typename Client::Quorum const& q,
         typename Client::Proposal const& p,
         elle::Option<T, typename Client::Quorum> const& value) override
  {
    throw paxos::Unavailable();
  }

  bool fail;
  elle::reactor::Barrier propose_barrier, accept_barrier;
  elle::reactor::Signal propose_signal, accept_signal;
};

template <typename T, typename Version, typename ServerId>
class NoConfirmationPeer
  : public Peer<T, Version, ServerId>
{
public:
  using Super = Peer<T, Version, ServerId>;
  using Client = paxos::Client<T, Version, ServerId>;

  using Super::Super;

  void
  confirm(typename paxos::Server<T, Version, ServerId>::Quorum const&,
          typename Client::Proposal const&) override
  {}
};

namespace quorum_divergence
{
  ELLE_TEST_SCHEDULED(one_of_three_thinks_quorum_changed)
  {
    using Server = paxos::Server<int, int, int>;
    using Client = paxos::Client<int, int, int>;
    using Peers = Client::Peers;
    Server server_1(11, {11, 12, 13});
    Server server_2(12, {11, 12, 13});
    Server server_3(13, {11, 12, 13});
    auto client = [&]
    {
      auto peers = Peers{};
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
      return paxos::Client<int, int, int>(1, std::move(peers));
    }();
    auto client_1_only = [&]
    {
      auto peers = Peers{};
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
      peers.emplace_back(
        std::make_unique<ProposeOnlyPeer<int, int, int>>(12, server_2));
      peers.emplace_back(
        std::make_unique<ProposeOnlyPeer<int, int, int>>(13, server_3));
      return paxos::Client<int, int, int>(2, std::move(peers));
    }();
    auto client_partial_23 = [&]
    {
      auto peers = Peers{};
      peers.emplace_back(std::make_unique<UnavailablePeer<int, int, int>>(11));
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
      return paxos::Client<int, int, int>(3, std::move(peers));
    }();
    ELLE_LOG("choose 1 for version 1")
      BOOST_CHECK(!client.choose(1, 1));
    //        11        12        13
    //   +---------+---------+---------+
    // 1 |    1    |    1    |    1    | -> 1
    //   |  1:1:1  |  1:1:1  |  1:1:1  |
    //   +---------+---------+---------+
    ELLE_LOG("choose quorum \\{11, 12\\} for version 2 on server 11 only")
      BOOST_CHECK_THROW(client_1_only.choose(2, Client::Quorum{11, 12}),
                        elle::athena::paxos::TooFewPeers);
    //        11        12        13
    //   +---------+---------+---------+
    // 2 | {11,12} |         |         |
    //   |  2:1:2  |  2:1:2  |  2:1:2  |
    //   +---------+---------+---------+
    // 1 |    1    |    1    |    1    | -> 1
    //   |  1:1:1  |  1:1:1  |  1:1:1  |
    //   +---------+---------+---------+
    ELLE_LOG("choose 2 for version 2")
      BOOST_CHECK(!client_partial_23.choose(2, 2));
    //        11        12        13
    //   +---------+---------+---------+
    // 2 | {11,12} |    2    |    2    | -> 2
    //   |  2:1:2  |  2:1:3  |  2:1:3  |
    //   +---------+---------+---------+
    // 1 |    1    |    1    |    1    | -> 1
    //   |  1:1:1  |  1:1:1  |  1:1:1  |
    //   +---------+---------+---------+
    ELLE_LOG("choose 3 for version 3")
      BOOST_CHECK(!client.choose(3, 3));
  }

  // Check that if one of two peers has the new quorum of 2 commited, but the
  // other still thinks they are 3, the newest quorum takes precedence.
  ELLE_TEST_SCHEDULED(one_of_three_knows_quorum_changed)
  {
    using Server = paxos::Server<int, int, int>;
    using Client = paxos::Client<int, int, int>;
    using Peers = Client::Peers;
    Server server_1(11, {11, 12, 13});
    Server server_2(12, {11, 12, 13});
    Server server_3(13, {11, 12, 13});
    ELLE_LOG("choose 1 for version 1");
    {
      auto peers = Peers{};
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
      auto client = paxos::Client<int, int, int>(1, std::move(peers));
        BOOST_TEST(!client.choose(1, 1));
    }
    //        11        12        13
    //   +---------+---------+---------+
    // 1 |    1    |    1    |    1    | -> 1
    //   |  1:1:1  |  1:1:1  |  1:1:1  |
    //   +---------+---------+---------+
    ELLE_LOG("choose \\{11,12\\} for version 2, only confirm on 11")
    {
      auto peers = Peers{};
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
      peers.emplace_back(
        std::make_unique<NoConfirmationPeer<int, int, int>>(12, server_2));
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(13, server_3));
      auto client = paxos::Client<int, int, int>(2, std::move(peers));
      client.choose(2, Client::Quorum{11, 12});
    }
    //        11        12        13
    //   +---------+
    //   |         |
    //   |         |
    //   +---------+---------+---------+
    // 2 | {11,12} | {11,12} | {11,12} | -> {11,12}
    //   |  2:1:2  |  2:1:2  |  2:1:2  |
    //   +---------+---------+---------+
    // 1 |    1    |    1    |    1    | -> 1
    //   |  1:1:1  |  1:1:1  |  1:1:1  |
    //   +---------+---------+---------+
    ELLE_LOG("get value")
    {
      auto peers = Peers{};
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(11, server_1));
      peers.emplace_back(std::make_unique<Peer<int, int, int>>(12, server_2));
      auto client = paxos::Client<int, int, int>(3, std::move(peers));
      BOOST_TEST(*client.get() == 1);
    }
  }
}

// FIXME: Doesn't test incomplete rounds serialization, like proposed but
// not-accepted rounds.
ELLE_TEST_SCHEDULED(serialization)
{
  using Server = paxos::Server<int, int, int>;
  using Client = paxos::Client<int, int, int>;
  using Peer = Peer<int, int, int>;
  using Peers = Client::Peers;
  elle::Buffer s1;
  elle::Buffer s2;
  ELLE_LOG("choose and serialize")
  {
    Server server_1(11, {11, 12});
    Server server_2(12, {11, 12});
    auto peers = Peers{};
    peers.emplace_back(new Peer(11, server_1));
    peers.emplace_back(new Peer(12, server_2));
    paxos::Client<int, int, int> client(1, std::move(peers));
    BOOST_CHECK(!client.choose(0, 0));
    BOOST_CHECK(!client.choose(1, 1));
    s1 = elle::serialization::json::serialize(server_1, false);
    s2 = elle::serialization::json::serialize(server_2, false);
  }
  ELLE_LOG("unserialize and choose")
  {
    elle::serialization::Context ctx;
    ctx.set<elle::Version>(elle::serialization_tag::version);
    auto server_1 =
      elle::serialization::json::deserialize<Server>(s1, false, ctx);
    auto server_2 =
      elle::serialization::json::deserialize<Server>(s2, false, ctx);
    auto peers = Peers{};
    peers.emplace_back(new Peer(11, server_1));
    peers.emplace_back(new Peer(12, server_2));
    paxos::Client<int, int, int> client(1, std::move(peers));
    BOOST_CHECK_EQUAL(client.choose(1, 0)->template get<int>(), 1);
    BOOST_CHECK(!client.choose(2, 2));
  }
}

ELLE_TEST_SCHEDULED(partial_state)
{
  using Server = paxos::Server<int, int, int>;
  using Client = paxos::Client<int, int, int>;
  using Peer = Peer<int, int, int>;
  using Peers = Client::Peers;
  std::vector<Server> servers
  {
    {11, {11, 12, 13}},
    {12, {11, 12, 13}},
    {13, {11, 12, 13}},
  };
  auto make_client = [&] (std::vector<bool> availability)
    {
      auto peers = Peers{};
      int i = 0;
      for (auto a: availability)
      {
        if (a)
          peers.emplace_back(new Peer(11 + i, servers[i]));
        else
          peers.emplace_back(new UnavailablePeer<int, int, int>(11 + i));
        ++i;
      }
      paxos::Client<int, int, int> client(1, std::move(peers));
      client.conflict_backoff(false);
      return client;
    };
  ELLE_LOG("select 0 for version 0")
    BOOST_CHECK(!make_client({true, true, true}).choose(0, 0));
  ELLE_LOG("select 1 for version 1 on \\{11, 12\\}")
    BOOST_CHECK(!make_client({true, true, false}).choose(1, 1));
  ELLE_LOG("propose 2 for version 2 on \\{11\\}")
    BOOST_CHECK_THROW(make_client({true, false, false}).choose(2, 2),
                      elle::athena::paxos::TooFewPeers);
  //        11        12        13
  //   +---------+---------+---------+
  // 2 |         |         |         |
  //   |  2:1:1  |         |         |
  //   +---------+---------+---------+
  // 1 |    1    |    1    |         | -> 1
  //   |  1:1:1  |  1:1:1  |         |
  //   +---------+---------+---------+
  // 0 |    0    |    0    |    0    | -> 0
  //   |  0:1:1  |  0:1:1  |  0:1:1  |
  //   +---------+---------+---------+
  ELLE_LOG("propose 2 for version 1 on \\{11, 13\\}")
    BOOST_CHECK(!make_client({true, false, true}).choose(1, 2));
  //        11        12        13
  //   +---------+---------+---------+
  // 2 |    2    |         |    2    | -> 2
  //   |  2:2:1  |         |  2:2:1  |
  //   +---------+---------+---------+
  // 1 |    1    |    1    |         | -> 1
  //   |  1:1:1  |  1:1:1  |         |
  //   +---------+---------+---------+
  // 0 |    0    |    0    |    0    | -> 0
  //   |  0:1:1  |  0:1:1  |  0:1:1  |
  //   +---------+---------+---------+
}

ELLE_TEST_SCHEDULED(non_partial_state)
{
  using Server = paxos::Server<int, int, int>;
  using Client = paxos::Client<int, int, int>;
  using Peers = Client::Peers;
  Server server(11, {11});
  auto make_client = [&] (std::unique_ptr<Peer<int, int, int>> p)
    {
      auto peers = Peers{};
      peers.emplace_back(std::move(p));
      Client client(1, std::move(peers));
      client.conflict_backoff(false);
      return client;
    };
  auto p1 = new InstrumentedPeer<int, int, int>(11, server);
  p1->propose_barrier.open();
  p1->accept_barrier.open();
  p1->confirm_barrier.open();
  auto c1 = make_client(std::unique_ptr<Peer<int, int, int>>(p1));
  auto p2 = new InstrumentedPeer<int, int, int>(11, server);
  p2->propose_barrier.open();
  auto c2 = make_client(std::unique_ptr<Peer<int, int, int>>(p2));
  c1.choose(0, 0);
  elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
  {
    scope.run_background(
      "client_1",
      [&]
      {
        p1->confirm_barrier.close();
        ELLE_LOG("choose 1");
        BOOST_CHECK(!c1.choose(1, 1));
        BOOST_CHECK_EQUAL(c1.get(), 1);
        p2->accept_barrier.open();
        p2->confirm_barrier.open();
      });
    scope.run_background(
      "client_2",
      [&]
      {
        elle::reactor::wait(p1->confirm_signal);
        BOOST_CHECK_EQUAL(c2.choose(1, 2)->get<int>(), 1);
      });
    elle::reactor::wait(p2->accept_signal);
    p1->confirm_barrier.open();
    elle::reactor::wait(scope);
  };
}

// Proposing on a wrong quorum used to commit the previous value - thus emptying
// the state - and then throw a WrongQuorum error, leaving an empty state which
// is intepreted as no value ever having been pushed.
ELLE_TEST_SCHEDULED(propose_wrong_quorum)
{
  std::vector<Server> servers
  {
    {11, {11, 12, 13}},
    {12, {11, 12, 13}},
    {13, {11, 12, 13}},
  };
  auto client = make_client(1, servers);
  client.choose(0, 1069);
  // Trigger a proposal with a wrong quorum on all servers.
  for (int i: irange(0, 3))
    BOOST_CHECK_THROW(
      make_client(1, servers | sliced(i, i + 1)).choose(1, 1076),
      Server::WrongQuorum);
  // Check the states where not emptied.
  BOOST_TEST(client.get() == 1069);
}

// Used to dereference an empty optional (this->_value)
ELLE_TEST_SCHEDULED(valueless_wrong_quorum)
{
  std::vector<Server> servers
  {
    {11, {11, 12}},
    {12, {11, 12}},
  };
  make_client(1, servers).choose(0, Server::Quorum{11, 12, 13});
  BOOST_CHECK_THROW(make_client(1, servers).choose(1, 1098),
                    Server::WrongQuorum);
}

ELLE_TEST_SCHEDULED(partial_conflict)
{
  std::vector<Server> servers
  {
    {11, {11, 12, 13}},
    {12, {11, 12, 13}},
    {13, {11, 12, 13}},
  };
  make_client(0, servers).choose(0, 1152);
  elle::reactor::Barrier confirm, pick_value;
  elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
  {
    scope.run_background(
      "pick_quorum",
      [&]
      {
        auto c = make_client(2, servers);
        static_cast<YAInstrumentedPeer&>(*c.peers()[0]).confirming().connect(
          [&] (Client::Proposal const& p)
          {
            pick_value.open();
            elle::reactor::wait(confirm);
          });
        c.choose(1, Server::Quorum{11, 12});
      });
    scope.run_background(
      "pick_value",
      [&]
      {
        elle::reactor::wait(pick_value);
        {
          auto c = make_client(1, servers);
          BOOST_CHECK_EQUAL(
            c.choose(1, 1187)->get<Server::Quorum>(),
            (Server::Quorum{11, 12}));
        }
        {
          auto c = make_client(2, servers | sliced(0, 2));
          BOOST_TEST(!c.choose(2, 1192));
          confirm.open();
        }
      });
    elle::reactor::wait(scope);
  };
}

ELLE_TEST_SCHEDULED(partial_interleave)
{
  std::vector<Server> servers;
  servers.reserve(3);
  servers.emplace_back(11, Server::Quorum{11, 12});
  servers.emplace_back(12, Server::Quorum{11, 12});
  elle::reactor::Barrier confirm, confirmed, pick_value;
  elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
  {
    scope.run_background(
      "pick_quorum",
      [&]
      {
        auto c = make_client(0, servers);
        static_cast<YAInstrumentedPeer&>(*c.peers()[0]).confirming().connect(
          [&] (Client::Proposal const& p)
          {
            if (p.sender == c.id())
            {
              pick_value.open();
              elle::reactor::wait(confirm);
              confirmed.open();
            }
          });
        c.choose(0, Server::Quorum{11, 12, 13});
      });
    scope.run_background(
      "pick_value",
      [&]
      {
        elle::reactor::wait(pick_value);
        {
          auto c = make_client(1, servers);
          BOOST_CHECK_THROW(c.choose(1, 1233), Server::WrongQuorum);
        }
        servers.emplace_back(13, Server::Quorum{11, 12, 13});
        confirm.open();
        elle::reactor::wait(confirmed);
        {
          auto c = make_client(2, servers);
          BOOST_CHECK(!c.choose(1, 1233));
        }
      });
    elle::reactor::wait(scope);
  };
}

ELLE_TEST_SCHEDULED(partial_in_progress)
{
  std::vector<Server> servers{
    {11, Server::Quorum{11, 12}},
    {12, Server::Quorum{11, 12}},
    {12, Server::Quorum{11, 12}},
  };
  elle::reactor::Barrier partial, confirming, reset;
  elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
  {
    scope.run_background(
      "create_partial",
      [&]
      {
        auto c = make_client(0, servers);
        static_cast<YAInstrumentedPeer&>(*c.peers()[0]).confirming().connect(
          [&] (Client::Proposal const& p)
          {
            if (p.sender == c.id())
              partial.open();
            elle::reactor::wait(reset);
          });
        c.choose(0, 1272);
      });
    scope.run_background(
      "pick_value",
      [&]
      {
        elle::reactor::wait(partial);
        auto c = make_client(1, servers);
        static_cast<YAInstrumentedPeer&>(*c.peers()[0]).confirming().connect(
          [&] (Client::Proposal const& p)
          {
            if (p.sender == c.id())
            {
              confirming.open();
              elle::reactor::wait(reset);
            }
          });
        c.choose(1, 1287);
      });
    scope.run_background(
      "reset",
      [&]
      {
        elle::reactor::wait(confirming);
        auto c = make_client(1, servers);
        static_cast<YAInstrumentedPeer&>(*c.peers()[0]).proposed().connect(
          [&] (Client::Proposal const& p)
          {
            if (p.sender == c.id())
            {
              reset.open();
              elle::reactor::scheduler().current()->terminate();
            }
          });
        c.choose(1, 1304);
      });
    elle::reactor::wait(scope);
  };
}

ELLE_TEST_SCHEDULED(self_conflict)
{
  std::vector<Server> servers{
    {11, Server::Quorum{11, 12, 13}},
    {12, Server::Quorum{11, 12, 13}},
    {13, Server::Quorum{11, 12, 13}},
  };
  elle::reactor::Barrier first, second;
  elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
  {
    scope.run_background(
      "first",
      [&]
      {
        auto c = make_client(0, servers);
        static_cast<YAInstrumentedPeer&>(*c.peers()[0]).accepting().connect(
          [&] (Client::Proposal const& p)
          {
            elle::reactor::wait(first);
          });
        static_cast<YAInstrumentedPeer&>(*c.peers()[1]).accepting().connect(
          [&] (Client::Proposal const& p)
          {
            elle::reactor::wait(first);
          });
        static_cast<YAInstrumentedPeer&>(*c.peers()[2]).accepted().connect(
          [&] (Client::Proposal const& p)
          {
            second.open();
          });
        auto r = c.choose(0, 1331);
        BOOST_REQUIRE(r);
        BOOST_CHECK_EQUAL(r->get<int>(), 1331);
      });
    scope.run_background(
      "second",
      [&]
      {
        elle::reactor::wait(second);
        auto c = make_client(0, servers);
        auto r = c.choose(0, 1349);
        BOOST_REQUIRE(r);
        BOOST_CHECK_EQUAL(r->get<int>(), 1331);
        first.open();
      });
    elle::reactor::wait(scope);
  };
}

ELLE_TEST_SCHEDULED(self_conflict2)
{
  std::vector<Server> servers{
    {11, Server::Quorum{11, 12, 13}},
    {12, Server::Quorum{11, 12, 13}},
    {13, Server::Quorum{11, 12, 13}},
  };
  elle::reactor::Barrier first, second;
  elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
  {
    scope.run_background(
      "first",
      [&]
      {
        auto c = make_client(0, servers);
        static_cast<YAInstrumentedPeer&>(*c.peers()[0]).accepting().connect(
          [&] (Client::Proposal const& p)
          {
            elle::reactor::wait(first);
          });
        static_cast<YAInstrumentedPeer&>(*c.peers()[1]).accepting().connect(
          [&] (Client::Proposal const& p)
          {
            elle::reactor::wait(first);
          });
        static_cast<YAInstrumentedPeer&>(*c.peers()[2]).accepting().connect(
          [&] (Client::Proposal const& p)
          {
            second.open();
            elle::reactor::wait(first);
          });
        auto r = c.choose(0, 1389);
        BOOST_REQUIRE(r);
        BOOST_CHECK_EQUAL(r->get<int>(), 1400);
      });
    scope.run_background(
      "second",
      [&]
      {
        elle::reactor::wait(second);
        auto c = make_client(0, servers);
        auto r = c.choose(0, 1400);
        BOOST_CHECK(!r);
        first.open();
      });
    elle::reactor::wait(scope);
  };
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
  suite.add(BOOST_TEST_CASE(propose_before_current_proposal_acceptation),
            0, valgrind(1));
  suite.add(BOOST_TEST_CASE(serialization), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(partial_state), 0, valgrind(5));
  suite.add(BOOST_TEST_CASE(non_partial_state), 0, valgrind(5));
  suite.add(BOOST_TEST_CASE(partial_conflict), 0, valgrind(5));
  suite.add(BOOST_TEST_CASE(partial_interleave), 0, valgrind(5));
  suite.add(BOOST_TEST_CASE(partial_in_progress), 0, valgrind(5));
  suite.add(BOOST_TEST_CASE(self_conflict), 0, valgrind(5));
  suite.add(BOOST_TEST_CASE(self_conflict2), 0, valgrind(5));
  {
    auto quorum = BOOST_TEST_SUITE("quorum");
    suite.add(quorum);
    quorum->add(BOOST_TEST_CASE(elect_extend), 0, valgrind(1));
    quorum->add(BOOST_TEST_CASE(elect_shrink), 0, valgrind(1));
    quorum->add(BOOST_TEST_CASE(evict_down_lag_behind), 0, valgrind(1));
    {
      auto divergence = BOOST_TEST_SUITE("divergence");
      using namespace quorum_divergence;
      quorum->add(divergence);
      divergence->add(
        BOOST_TEST_CASE(one_of_three_thinks_quorum_changed), 0, valgrind(1));
      divergence->add(
        BOOST_TEST_CASE(one_of_three_knows_quorum_changed), 0, valgrind(1));
    }
    quorum->add(BOOST_TEST_CASE(propose_wrong_quorum), 0, valgrind(5));
    quorum->add(BOOST_TEST_CASE(valueless_wrong_quorum), 0, valgrind(5));
  }
}
