#include <iostream>

#include <elle/cryptography/KeyPair.hh>
#include <elle/Buffer.hh>
#include <elle/serialize/Serializer.hh>

#include <nucleus/neutron/Access.hh>
#include <nucleus/proton/ImprintBlock.hh>

#include <tests/unit/unit.hh>

#define CHECK(call)                                                     \
  if (call != elle::Status::Ok) { assert(false); } else

TEST_SERIALIZE_FINALIZE(nucleus::proton, ImprintBlock)

namespace test
{
  void test()
  {
    {
      nucleus::proton::Network network("test");

      elle::cryptography::KeyPair kp(elle::cryptography::KeyPair::generate());

      test::ImprintBlock blk(network,
                             nucleus::neutron::ComponentObject,
                             kp.K);

      nucleus::proton::Address addr(blk.bind());

      assert(addr != nucleus::proton::Address::null());
      assert(addr != nucleus::proton::Address::some());

      blk.validate(addr);

      elle::Buffer buf;

      {
        auto writer = buf.writer();

        writer << elle::serialize::concrete(blk);
      }

      {
        auto reader = buf.reader();

        test::ImprintBlock blk_copy;

        reader >> elle::serialize::concrete(blk_copy);

        blk_copy.validate(addr);
      }
    }
  }
}

int main(int, char** argv)
{
  try
    {
      test::test();

      std::cout << "tests done.\n";
      return 0;
    }
  catch (std::exception const& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      if (reactor::Exception const* re =
          dynamic_cast<reactor::Exception const*>(&e))
        std::cerr << re->backtrace() << std::endl;

      return 1;
    }
}
