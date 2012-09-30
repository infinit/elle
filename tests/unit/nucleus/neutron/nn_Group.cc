#include <iostream>

#include <elle/Elle.hh>
#include <elle/print.hh>
#include <elle/cryptography/KeyPair.hh>

#include <nucleus/neutron/Group.hh>
#include <nucleus/neutron/Ensemble.hh>
#include <nucleus/neutron/Fellow.hh>

#include <elle/idiom/Open.hh>
#include <elle/log.hh>

#define CHECK(call) if (call != elle::Status::Ok) { show(); assert(false); } else

ELLE_LOG_COMPONENT("infinit.tests.nucleus.neutron.Group");

void test()
{
  elle::cryptography::KeyPair owner;

  ELLE_TRACE("Generate owner key pair")
    {
      CHECK(owner.Generate());
    }

  nucleus::proton::Network network("test");

  nucleus::neutron::Group group(network, owner.K, "everybody");

  elle::cryptography::KeyPair pass;

  ELLE_TRACE("Generate the group's pass")
    {
      CHECK(pass.Generate());
    }

  nucleus::neutron::Ensemble ensemble(network, owner.K);

  ELLE_TRACE("Add subjects in the ensemble")
    for (int i = 0; i < 5; i++)
      {
        elle::cryptography::KeyPair kp;

        CHECK(kp.Generate());

        nucleus::neutron::Subject subject(kp.K);

        ensemble.add(
          std::move(std::unique_ptr<nucleus::neutron::Fellow>(
            new nucleus::neutron::Fellow(subject))));

        assert(group.state() == nucleus::proton::StateDirty);
      }

  ELLE_TRACE("Update the ensemble with the private pass")
    {
      ensemble.update(pass.k);
    }

  ELLE_TRACE("Bind the ensemble block");
  nucleus::proton::Address ensemble_address(ensemble.bind());

  ELLE_TRACE("Upgrade the group")
    {
      nucleus::neutron::Token manager_token(group.manager_K(), pass.k);

      group.upgrade(ensemble_address, pass.K, manager_token);

      assert(group.state() == nucleus::proton::StateDirty);
    }

  ELLE_TRACE("Seal the group")
    {
      group.seal(owner.k);

      assert(group.state() == nucleus::proton::StateConsistent);
    }

  ELLE_TRACE("Bind the group block");
  nucleus::proton::Address group_address(group.bind());

  elle::utility::Buffer buffer;

  ELLE_TRACE("Serialize the group and ensemble blocks")
    {
      buffer.Writer() << group << ensemble;
    }
  {
  nucleus::neutron::Group g;
  nucleus::neutron::Ensemble e;

  ELLE_TRACE("Deserialize the group and ensemble blocks")
    {
      buffer.Reader() >> g >> e;
    }

  ELLE_TRACE("Validate the group")
    g.validate(group_address);

  ELLE_TRACE("Validate the ensemble")
    e.validate(ensemble_address);
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
