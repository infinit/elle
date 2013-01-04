#include <elle/concurrency/Scheduler.hh>
#include <elle/nat/Nat.hh>


int
main(int , const char *[])
{
    elle::nat::NAT a(elle::concurrency::scheduler());

    (void)a;
    std::cout << "tests done." << std::endl;
    return 0;
}
