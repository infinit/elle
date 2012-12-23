#include <cryptography/random.hh>
#include <elle/Exception.hh>

int
main(int, const char *[])
{
    try {
    infinit::cryptography::random::initialize();
    infinit::cryptography::random::clean();
    }
    catch (elle::Exception const &e)
    {
        std::cout << e.what() << std::endl;
        return (1);
    }
    std::cout << "tests done." << std::endl;
    return (0);
}
