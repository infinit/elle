#include <sstream>
#include <elle/nat/Nat.hh>
#include <elle/concurrency/Scheduler.hh>

#include <reactor/network/buffer.hh>
#include <reactor/sleep.hh>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

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
    , _public_endpoint{"", 0}
{
}

Hole::Hole(reactor::Scheduler &sched,
           std::string const &hostname,
           int port)
    : _handle(new rnet::UDPSocket(sched, hostname, port))
    , _public_endpoint{"", 0}
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
    std::vector<std::string> splited;

    ss << "local " << this->_handle->local_endpoint() << std::endl;
    resp.resize(512);
    this->_handle->write(rnet::Buffer(ss.str()));
    this->_handle->read_some(rnet::Buffer(resp));

    boost::split(splited, resp, boost::is_any_of(" :"));
    // The format of the msg returned by the server is:
    // local hostname:port
    // so,after the split we have:
    // ["local", "hostname", "port"]
    this->_public_endpoint = std::make_pair(splited[1], std::stoi(splited[2]));
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

std::pair<std::string, uint16_t>
NAT::punch(std::string const &hostname,
      int port)
{
    Hole h(sched, hostname, port);

    h.drill();
    return h.public_endpoint();
}


// class NAT }}}

} /* nat */
} /* elle */
