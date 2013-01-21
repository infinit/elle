#include <elle/print.hh>
#include <elle/idiom/Open.hh>
#include <elle/log.hh>

#include <cryptography/KeyPair.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

#include <nucleus/neutron/Group.hh>
#include <nucleus/neutron/Ensemble.hh>
#include <nucleus/neutron/Fellow.hh>

#include <iostream>

#define CHECK(call) if (call != elle::Status::Ok) { assert(false); } else

ELLE_LOG_COMPONENT("infinit.tests.nucleus.neutron.Group");

void test1()
{
  cryptography::KeyPair owner(cryptography::KeyPair::generate(1024));

  nucleus::proton::Network network("test");

  nucleus::neutron::Group group(network, owner.K(), "everybody");

  cryptography::KeyPair pass(cryptography::KeyPair::generate(2048));

  /* XXX
  nucleus::neutron::Ensemble ensemble(network, owner.K());

  ELLE_TRACE("Add subjects in the ensemble")
    for (int i = 0; i < 5; i++)
      {
        cryptography::KeyPair kp(cryptography::KeyPair::generate(1024));

        nucleus::neutron::Subject subject(kp.K());

        ensemble.add(
          std::move(std::unique_ptr<nucleus::neutron::Fellow>(
                      new nucleus::neutron::Fellow(
                        subject, nucleus::neutron::Token::null()))));

        assert(group.state() == nucleus::proton::State::dirty);
      }

  ELLE_TRACE("Update the ensemble with the private pass")
    {
      ensemble.update(pass.k());
    }

  ELLE_TRACE("Bind the ensemble block");
  nucleus::proton::Address ensemble_address(ensemble.bind());

  ELLE_TRACE("Upgrade the group")
    {
      nucleus::neutron::Token manager_token(pass.k(), group.manager_K());

      group.upgrade(ensemble_address, pass.K(), manager_token);

      assert(group.state() == nucleus::proton::State::dirty);
    }

  ELLE_TRACE("Seal the group")
    {
      group.seal(owner.k());

      assert(group.state() == nucleus::proton::State::consistent);
    }

  ELLE_TRACE("Bind the group block");
  nucleus::proton::Address group_address(group.bind());

  elle::Buffer buffer;

  ELLE_TRACE("Serialize the group and ensemble blocks")
    {
      buffer.writer() << group << ensemble;
    }
  {
  nucleus::neutron::Group g;
  nucleus::neutron::Ensemble e;

  ELLE_TRACE("Deserialize the group and ensemble blocks")
    {
      buffer.reader() >> g >> e;
    }

  ELLE_TRACE("Validate the group")
    g.validate(group_address);

  ELLE_TRACE("Validate the ensemble")
    e.validate(ensemble_address);
  }
  */
}

void test2()
{
  cryptography::KeyPair owner(cryptography::KeyPair::generate(1024));

  nucleus::proton::Network network("test");

  nucleus::neutron::Group group(network, owner.K(), "suce mon cul");

  nucleus::neutron::Token token(group.manager_token());

  cryptography::PrivateKey k{
    token.extract<cryptography::PrivateKey>(owner.k())};
}

int main(int, char** argv)
{
  try
    {
      test1();
      test2();

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
