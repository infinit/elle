#include <agent/Agent.hh>

#include <elle/io/Console.hh>
#include <elle/log.hh>
#include <elle/standalone/Report.hh>

#include <lune/Lune.hh>

#include <common/common.hh>

#include <elle/idiom/Close.hh>
# include <boost/filesystem.hpp>
#include <elle/idiom/Open.hh>

#include <Infinit.hh>

ELLE_LOG_COMPONENT("infinit.agent.Agent");

namespace agent
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "agent";

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

    // disable the meta logging.
    if (elle::radix::Meta::Disable() == elle::Status::Error)
      escape("unable to disable the meta logging");

    //
    // load the identity.
    //
    {
      // does the identity exist.
      if (Agent::Identity.Exist(Infinit::User) == elle::Status::False)
        escape("the user identity does not seem to exist");

      std::ifstream identity_file(common::watchdog::identity_path());
      if (identity_file.good())
        {
          std::getline(identity_file, Agent::meta_token);
          std::string clear_identity;
          std::getline(identity_file, clear_identity);
          ELLE_TRACE("got token: %s", Agent::meta_token);
          if (Agent::Identity.Restore(clear_identity) == elle::Status::Error)
            escape("unable to restore the identity");
        }
      else
        {
          ELLE_TRACE("Cannot load identity from watchdog %s",
                         common::watchdog::identity_path());
          elle::String        pass;
          // prompt the user for the passphrase.
          /* XXX[to change to a better version where we retrieve the passphrase
           * from the watchdog]
          prompt = "Enter passphrase for keypair '" + Infinit::User + "': ";

          if (elle::io::Console::Input(
                pass,
                prompt,
                elle::io::Console::OptionPassword) == elle::Status::Error)
            escape("unable to read the input");
          */
          // XXX[temporary fix]

          // load the identity.
          if (Agent::Identity.Load(Infinit::User) == elle::Status::Error)
            escape("unable to load the identity");

          // verify the identity.
          if (Agent::Identity.Validate(Infinit::Authority) == elle::Status::Error)
            escape("the identity seems to be invalid");

          // decrypt the identity.
          if (Agent::Identity.Decrypt(pass) == elle::Status::Error)
            escape("unable to decrypt the identity");
        }
    }

    //
    // create a subject representing the user.
    //
    {
      // create the subject.
      if (Agent::Subject.Create(Agent::Identity.pair.K) == elle::Status::Error)
        escape("unable to create the user's subject");
    }

    // enable the meta logging.
    if (elle::radix::Meta::Enable() == elle::Status::Error)
      escape("unable to enable the meta logging");

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
