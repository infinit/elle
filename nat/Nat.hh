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

class Hole
{
private:
    u_ptr<rnet::UDPSocket> _handle; 
public:
    Hole(reactor::Scheduler &sched,
         std::string const &hostname,
         std::string const &port);

    Hole(reactor::Scheduler &sched,
         std::string const &hostname,
         int port);

    ~Hole();

public:
    void
    sayHello(void);

    void
    exchangeIP(void);

    void
    drill(void);

public:
    u_ptr<rnet::UDPSocket> &&
    getPunchedHandle()
    {
        return std::move(this->_handle);
    }
};

class KeepAlive
{
private:
    reactor::Scheduler    &sched;
    u_ptr<rnet::UDPSocket> handle;
    bool                  running = true;

public:
    KeepAlive(reactor::Scheduler &sched,
              u_ptr<rnet::UDPSocket> &&s);
    ~KeepAlive();

public:
    void
    run(void);

    void
    stop(void);

};

class NAT
{
public:

public:
    NAT(reactor::Scheduler &s);
    ~NAT();

public:
    void
    punch(std::string const &hostname,
          int port);

private:
    reactor::Scheduler &sched;
    u_ptr<reactor::Thread> alive;
};

} /* nat */
} /* elle */

#endif /* end of include guard: HOLEPUNCHER_LFFVGYPW */
