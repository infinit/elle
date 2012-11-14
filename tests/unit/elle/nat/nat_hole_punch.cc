#include <elle/nat/Nat.hh>
#include <elle/concurrency/Scheduler.hh>

#include <reactor/sleep.hh>

int
Main(void)
{
    elle::nat::NAT a(elle::concurrency::scheduler());

    a.punch("localhost", 9999);
    while (1)
    {
        reactor::Sleep s(elle::concurrency::scheduler(),
                         boost::posix_time::seconds(5));
        s.run();
        std::printf("%s:%d\n", __PRETTY_FUNCTION__, __LINE__);
    }
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
                            std::cout << "Finished test" << std::endl;
                         });
    sched.run();
    return 0;
}
