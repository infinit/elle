#include <sstream>
#include <elle/nat/Nat.hh>
#include <elle/concurrency/Scheduler.hh>

#include <reactor/network/buffer.hh>
#include <reactor/sleep.hh>

namespace elle {
namespace nat {

namespace {

namespace ec = elle::concurrency;
namespace rnet = reactor::network;

}

// Class Hole {{{

Hole::Hole(reactor::Scheduler &sched,
           std::string const &hostname,
           std::string const &port)
    : _handle(new rnet::UDPSocket(sched, hostname, port))
{
}

Hole::Hole(reactor::Scheduler &sched,
           std::string const &hostname,
           int port)
    : _handle(new rnet::UDPSocket(sched, hostname, port))
{
}

Hole::~Hole()
{
}

void
Hole::sayHello(void)
{
    char msg[] = "hello\n";
    std::string resp;

    resp.resize(512);
    this->_handle->write(rnet::Buffer(msg, sizeof(msg)));
}

void
Hole::exchangeIP(void)
{
    std::string resp;
    std::stringstream ss;

    ss << "local " << this->_handle->local_endpoint() << std::endl;
    resp.resize(512);
    this->_handle->write(rnet::Buffer(ss.str()));
    this->_handle->read_some(rnet::Buffer(resp));
}

void
Hole::drill(void)
{
    this->sayHello();
    this->exchangeIP();
}

// class Hole }}}

// class KeepAlive {{{

KeepAlive::KeepAlive(reactor::Scheduler& sched,
                     u_ptr<rnet::UDPSocket>&& s)
    : sched(sched)
    , handle(std::move(s))
{
}

KeepAlive::~KeepAlive()
{
}

void
KeepAlive::run(void)
{
    std::string  buf;

    buf.resize(128);
    while (this->running == true)
    {
        reactor::Sleep s(this->sched, boost::posix_time::seconds(2));

        handle->write(rnet::Buffer(std::string("ping\n")));
        handle->read_some(rnet::Buffer(buf));
        s.run();
    }
}

void
KeepAlive::stop(void)
{
    this->running = false;
}

// class KeepAlive }}}

// class NAT {{{

NAT::NAT(reactor::Scheduler &s)
    : sched(s)
    , alive()
{
}

NAT::~NAT()
{
    if (this->alive)
        this->alive->terminate();
}

void
NAT::punch(std::string const &hostname,
      int port)
{
    Hole h(sched, hostname, port);

    h.drill();
    rnet::UDPSocket *ptr = h.getPunchedHandle().release(); /*Hack*/
    this->alive = u_ptr<reactor::Thread>(new reactor::Thread(
        this->sched,
        "Keep Alive UDP Hole Punching",
        [this, ptr]
        {
           u_ptr<KeepAlive> ka(
               new KeepAlive(this->sched, u_ptr<rnet::UDPSocket>(ptr))
           );
           ka->run();
        }
    ));
}


// class NAT }}}

} /* nat */
} /* elle */
