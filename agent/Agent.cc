#include <agent/Agent.hh>

#include <elle/io/Console.hh>

#include <lune/Lune.hh>

#include <common/common.hh>

#include <boost/filesystem.hpp>

#include <Infinit.hh>

ELLE_LOG_COMPONENT("infinit.agent.Agent");

namespace agent
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// the user's identity.
  ///
  lune::Identity                Agent::Identity;

  ///
  /// this variable represents the user subject.
  ///
  nucleus::neutron::Subject Agent::Subject;

  elle::String                  Agent::meta_token;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the agent.
  ///
  elle::Status          Agent::Initialize()
  {
    elle::String        prompt;

    //
    // load the identity.
    //
    {
      // does the identity exist.
      if (lune::Identity::exists(Infinit::User) == false)
        throw elle::Exception("the user identity does not seem to exist");

      std::ifstream identity_file(common::infinit::identity_path(Infinit::User));
      if (identity_file.good())
        {
          std::getline(identity_file, Agent::meta_token);
          std::string clear_identity;
          std::getline(identity_file, clear_identity);
          ELLE_TRACE("got token: %s", Agent::meta_token);
          if (Agent::Identity.Restore(clear_identity) == elle::Status::Error)
            throw elle::Exception("unable to restore the identity");
        }
      else
        {
          ELLE_TRACE("Cannot load identity from  %s",
                     common::infinit::identity_path(Infinit::User));
          elle::String        pass;
          // prompt the user for the passphrase.
          /* XXX[to change to a better version where we retrieve the passphrase
           * from the watchdog]
          prompt = "Enter passphrase for keypair '" + Infinit::User + "': ";

          if (elle::io::Console::Input(
                pass,
                prompt,
                elle::io::Console::OptionPassword) == elle::Status::Error)
            throw elle::Exception("unable to read the input");
          */
          // XXX[temporary fix]

          // load the identity.
          Agent::Identity.load(Infinit::User);

          // verify the identity.
          if (Agent::Identity.Validate(Infinit::authority())
              == elle::Status::Error)
            throw elle::Exception("the identity seems to be invalid");

          // decrypt the identity.
          if (Agent::Identity.Decrypt(pass) == elle::Status::Error)
            throw elle::Exception("unable to decrypt the identity");
        }
    }

    //
    // create a subject representing the user.
    //
    {
      // create the subject.
      if (Agent::Subject.Create(Agent::Identity.pair().K()) == elle::Status::Error)
        throw elle::Exception("unable to create the user's subject");
    }

    return elle::Status::Ok;
  }

  ///
  /// this method cleans the agent.
  ///
  /// the components are recycled just to make sure the memory is
  /// released before the Meta allocator terminates.
  ///
  elle::Status          Agent::Clean()
  {
    // nothing to do.

    return elle::Status::Ok;
  }

}
