#include <iostream>

#include <elle/Elle.hh>
#include <elle/cryptography/KeyPair.hh>
#include <elle/utility/Buffer.hh>

#include <nucleus/Nucleus.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/proton/ImprintBlock.hh>

#define CHECK(call) if (call != elle::Status::Ok) { show(); assert(false); } else

int main()
{

  CHECK(elle::Elle::Initialize());
  CHECK(nucleus::Nucleus::Initialize());

    {
      nucleus::proton::Network network("test");

      elle::cryptography::KeyPair kp;
      CHECK(kp.Generate());

      printf("HERE\n");

      nucleus::proton::ImprintBlock blk(network,
                                        nucleus::neutron::ComponentObject,
                                        kp.K);

      printf("HERE\n");

      nucleus::proton::Address addr(blk.bind());

      printf("HERE\n");

      assert(addr != nucleus::proton::Address::Null);
      assert(addr != nucleus::proton::Address::Some);

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

  std::cout << "tests done.\n";

  CHECK(nucleus::Nucleus::Clean());
  CHECK(elle::Elle::Clean());

  return 0;
}

