#pragma once
#ifndef HOLEPUNCHER_LFFVGYPW
# define HOLEPUNCHER_LFFVGYPW

#include <elle/network/Locus.hh>

#include <reactor/fwd.hh>
#include <reactor/network/udp-socket.hh>

#include <algorithm>
#include <string>

namespace elle {
namespace nat {

namespace {

namespace ec = elle::concurrency;
namespace rnet = reactor::network;

}

template <class C>
using u_ptr = std::unique_ptr<C>;

template <class C>
using s_ptr = std::shared_ptr<C>;

// Class Hole {{{

class Hole
{
private:
    s_ptr<rnet::UDPSocket> _handle;

    std::pair<std::string, uint16_t> _public_endpoint;
    boost::asio::ip::udp::endpoint _endpoint;

public:
    Hole() = delete;
    Hole(Hole const& hole) = delete;
    Hole& operator =(Hole const& other) = delete;
    Hole& operator =(Hole&& other) = delete;
    Hole(Hole&& hole);
    ~Hole();

    Hole(reactor::Scheduler &sched,
         std::string const &hostname,
         std::string const &port);

    Hole(reactor::Scheduler &sched,
         std::string const &hostname,
         int port,
         int local_port = 0);

private:
    void
    say(std::string const);

    std::string const
    get();
public:
    void
    sayHello(void);

    void
    exchangeIP(void);

    void
    drill(void);

public:
    s_ptr<rnet::UDPSocket>
    punched_handle()
    {
        return this->_handle;
    }

    std::pair<std::string, uint16_t>
    public_endpoint()
    {
        return this->_public_endpoint;
    }
};

// class Hole }}}

// Class KeepAlive {{{

class KeepAlive
{
private:
    reactor::Scheduler    &sched;
    s_ptr<rnet::UDPSocket> handle;
    bool                  running = true;

public:
    KeepAlive(reactor::Scheduler &sched,
              s_ptr<rnet::UDPSocket> &&s);
    ~KeepAlive();

public:
    void
    run(void);

    void
    stop(void);

};

// Class KeepAlive }}}

// class NAT {{{

class NAT
{
public:

public:
    NAT(reactor::Scheduler &s);
    ~NAT();

public:
    Hole
    punch(std::string const &hostname,
          int port,
          int local_port = 0);

private:
    reactor::Scheduler &sched;
    u_ptr<reactor::Thread> alive;
};

// class NAT }}}

} /* nat */
} /* elle */

#endif /* end of include guard: HOLEPUNCHER_LFFVGYPW */
