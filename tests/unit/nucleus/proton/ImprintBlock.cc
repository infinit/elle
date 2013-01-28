#include <elle/Buffer.hh>
#include <elle/serialize/Serializer.hh>

#include <cryptography/KeyPair.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

#include <nucleus/neutron/Access.hh>
#include <nucleus/proton/ImprintBlock.hh>

#include <tests/unit/unit.hh>

#include <iostream>

#define CHECK(call)                                                     \
  if (call != elle::Status::Ok) { assert(false); } else

TEST_SERIALIZE_FINALIZE(nucleus::proton, ImprintBlock)

namespace test
{
  void test()
  {
    {
      nucleus::proton::Network network("test");

      cryptography::KeyPair kp =
        cryptography::KeyPair::generate(cryptography::Cryptosystem::rsa, 1024);

      test::ImprintBlock blk(network,
                             nucleus::neutron::ComponentObject,
                             kp.K());

      nucleus::proton::Address addr(blk.bind());

      assert(addr != nucleus::proton::Address::null());

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
