#include <elle/print.hh>
#include <elle/Buffer.hh>

#include <cryptography/KeyPair.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>

#include <iostream>

#define CHECK(call) if (call != elle::Status::Ok) { assert(false); } else

int main()
{
  cryptography::KeyPair kp =
    cryptography::KeyPair::generate(cryptography::Cryptosystem::rsa, 2048);

  nucleus::proton::Network network("name");
  nucleus::neutron::Object blk(network,
                               kp.K(),
                               nucleus::neutron::Genre::directory);

  cryptography::Digest fingerprint;

  CHECK(blk.Seal(kp.k(), fingerprint));

  nucleus::proton::Address addr(blk.bind());

  elle::Buffer buf;
  buf.writer() << blk;

    {
      nucleus::neutron::Object blk_copy;
      buf.reader() >> blk_copy;

      blk_copy.validate(addr, fingerprint);

      assert(blk.owner_subject() == blk_copy.owner_subject());
    }

  std::cout << "tests done.\n";
  return 0;
}
