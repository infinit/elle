#include <iostream>

#include <elle/Elle.hh>
#include <elle/print.hh>
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

  nucleus::proton::Network network("name");
  nucleus::neutron::Object blk(network,
                               kp.K,
                               nucleus::neutron::GenreDirectory);

  CHECK(blk.Seal(kp.k, nucleus::neutron::Access::Null));

  nucleus::proton::Address addr(blk.bind());

  std::string unique;
  static_cast<elle::concept::Uniquable<> const&>(blk).Save(unique);
  elle::print("unique string:", unique);

  elle::utility::Buffer buf;
  buf.Writer() << blk;

    {
      nucleus::neutron::Object blk_copy;
      buf.Reader() >> blk_copy;

      blk_copy.validate(addr, nucleus::neutron::Access::Null);

      assert(blk.owner_subject() == blk_copy.owner_subject());
    }

  std::cout << "tests done.\n";
  return 0;
}
