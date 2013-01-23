#include <elle/nat/Nat.hh>
#include <elle/concurrency/Scheduler.hh>

#include <reactor/sleep.hh>

int
Main(void)
{
    elle::nat::NAT a(elle::concurrency::scheduler());

    elle::nat::Hole h = a.punch("infinit.im", 9999);
    std::cout << h.public_endpoint().first << ":" << h.public_endpoint().second << std::endl;
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
