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

      elle::cryptography::KeyPair kp;

      CHECK(kp.Generate());

      nucleus::proton::ImprintBlock blk;

      CHECK(blk.Create(kp.K));

      nucleus::proton::Address addr;

      CHECK(blk.Bind(addr));

      assert(addr != nucleus::proton::Address::Null);
      assert(addr != nucleus::proton::Address::Some);

      CHECK(blk.Validate(addr));

      elle::utility::Buffer buf;

        {
          auto writer = buf.Writer();

          writer << blk;
        }

        {
          auto reader = buf.Reader();

          nucleus::proton::ImprintBlock blk_copy;

          reader >> blk_copy;

          CHECK(blk_copy.Validate(addr));
        }
    }

  std::cout << "tests done.\n";

  CHECK(nucleus::Nucleus::Clean());
  CHECK(elle::Elle::Clean());

  return 0;
}

