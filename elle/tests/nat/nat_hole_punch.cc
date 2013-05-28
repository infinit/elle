#include <elle/nat/Nat.hh>
#include <elle/concurrency/Scheduler.hh>

#include <reactor/sleep.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/resolve.hh>

#include <iostream>
#include <limits>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

int
Main(void)
{
    elle::nat::NAT a(elle::concurrency::scheduler());

    elle::nat::Hole h = a.punch("development.infinit.io", 9999, 54545);
    std::cout << h.public_endpoint().first << ":" << h.public_endpoint().second << std::endl;
    // This stuff is usefull for debug. uncomment if you need it
    //{
    //    std::string in;
    //    std::vector<std::string> v;
    //    auto handle = h.punched_handle();

    //    std::cin >> in;
    //    boost::split(v, in, boost::is_any_of(":\n"));
    //    std::string ip = v[0];
    //    std::string port = v[1];
    //    boost::asio::ip::udp::endpoint remote =
    //        reactor::network::resolve_udp(elle::concurrency::scheduler(), ip, port);
    //    std::string msg{"Chie du foutre"};
    //    while (1)
    //    {
    //        std::string from_remote;
    //        boost::asio::ip::udp::endpoint ignore;

    //        from_remote.reserve(512);
    //        handle->send_to(reactor::network::Buffer(msg + "\n"), remote);
    //        handle->receive_from(reactor::network::Buffer(from_remote), remote);
    //        std::cout << from_remote;
    //    }
    //}
    return 0;
}

int
main(int , const char *[])
{
    reactor::Scheduler &sched = elle::concurrency::scheduler();
    reactor::Thread main(sched,
                         "test main",
                         []
                         {
                            Main();
                            std::cout << "tests done." << std::endl;
                         });
    sched.run();
    return 0;
}
