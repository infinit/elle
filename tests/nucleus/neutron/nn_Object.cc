#include <iostream>

#include <elle/Elle.hh>
#include <elle/cryptography/KeyPair.hh>
#include <elle/utility/Buffer.hh>

#include <nucleus/Nucleus.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>

#define CHECK(call) if (call != elle::Status::Ok) { show(); assert(false); } else

int main()
{

  CHECK(elle::Elle::Initialize());
  CHECK(nucleus::Nucleus::Initialize());

  elle::cryptography::KeyPair kp;

  CHECK(kp.Generate());

  nucleus::neutron::Object blk;

  CHECK(blk.Create(nucleus::neutron::GenreDirectory, kp.K));

  CHECK(blk.Seal(kp.k, nucleus::neutron::Access::Null));

  nucleus::proton::Address addr;

  CHECK(blk.Bind(addr));

  elle::utility::Buffer buf;

    {
      auto writer = buf.Writer();

      writer << blk;
    }

    {
      auto reader = buf.Reader();

      nucleus::neutron::Object blk_copy;

      reader >> blk_copy;

      CHECK(blk_copy.Validate(addr, nucleus::neutron::Access::Null));
    }

  std::cout << "tests done.\n";
  return 0;
}


