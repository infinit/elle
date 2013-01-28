#include <elle/utility/Suffixes.hh>
#include <iostream>
#include <elle/assert.hh>

int
main()
{
    using namespace elle::suffix;

    {
        auto val = 1010_bits;

        std::cout << val << std::endl;
        ELLE_ASSERT(val.to_ulong() == 10);
    }
    {
        auto val = 01010_bits;

        std::cout << val << std::endl;
        ELLE_ASSERT(val.to_ulong() == 10);
    }
    {
        auto val = 11111111111111111111111111111111_bits;

        std::cout << val << std::endl;
        ELLE_ASSERT(val.to_ulong() == 4294967296 - 1);
    }
    std::cout << "tests done." << std::endl;
    return (0);
}
