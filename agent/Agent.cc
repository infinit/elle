
#include <boost/filesystem.hpp>

#include <agent/Agent.hh>

#include <elle/io/Console.hh>
#include <elle/standalone/Report.hh>
#include <elle/idiom/Open.hh>

#include <lune/Lune.hh>

#include <Infinit.hh>

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

      // XXX to be improved with security in mind
      elle::io::Path home;
      home.Create(lune::Lune::Home);
      boost::filesystem::path path{home.str()};
      path /= "identity.wtg";

      std::ifstream identity_file(path.string());
      if (identity_file.good())
        {
          std::getline(identity_file, Agent::meta_token);
          std::string clear_identity;
          std::getline(identity_file, clear_identity);
          Agent::Identity.Restore(clear_identity);
        }
      else
        {
          elle::log::warn("Cannot load identity from watchdog ", path.string());
          elle::String        pass;
          // prompt the user for the passphrase.
          /* XXX[to change to a better version where we retrieve the passphrase from
                 the watchdog]
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
