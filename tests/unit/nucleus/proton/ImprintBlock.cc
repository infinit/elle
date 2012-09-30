#include <iostream>

#include <elle/Elle.hh>
#include <elle/cryptography/KeyPair.hh>
#include <elle/utility/Buffer.hh>

#include <nucleus/neutron/Access.hh>
#include <nucleus/proton/ImprintBlock.hh>

#define CHECK(call) if (call != elle::Status::Ok) { show(); assert(false); } else

void test()
{
    {
      nucleus::proton::Network network("test");

      elle::cryptography::KeyPair kp;
      CHECK(kp.Generate());

      nucleus::proton::ImprintBlock blk(network,
                                        nucleus::neutron::ComponentObject,
                                        kp.K);

      nucleus::proton::Address addr(blk.bind());

      assert(addr != nucleus::proton::Address::null);
      assert(addr != nucleus::proton::Address::some);

      blk.validate(addr);

      elle::utility::Buffer buf;

        {
          auto writer = buf.Writer();

          writer << elle::serialize::concrete(blk);
        }

        {
          auto reader = buf.Reader();

          nucleus::proton::ImprintBlock blk_copy;

          reader >> elle::serialize::concrete(blk_copy);

          blk_copy.validate(addr);
        }
    }
}

int main(int, char** argv)
{
  try
    {
      CHECK(elle::Elle::Initialize());

      test();

      CHECK(elle::Elle::Clean());

      std::cout << "tests done.\n";
      return 0;
    }
  catch (std::exception const& e)
    {
      // XXX
      show();

      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      if (reactor::Exception const* re =
          dynamic_cast<reactor::Exception const*>(&e))
        std::cerr << re->backtrace() << std::endl;

      return 1;
    }
}
