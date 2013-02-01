#include "../State.hh"
#include "../MetricReporter.hh"

#include <common/common.hh>

#include <lune/Descriptor.hh>
#include <lune/Identity.hh>
#include <lune/Set.hh>
#include <lune/Lune.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Porcupine.hh>
#include <nucleus/neutron/Genre.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Trait.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Access.hh>

#include <hole/storage/Directory.hh>

#include <elle/format/json.hh>
#include <elle/io/Piece.hh>
#include <elle/os/path.hh>
#include <elle/Passport.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

#include <QString>
#include <QByteArray>
#include <QLocalSocket>
#include <QProcess>

ELLE_LOG_COMPONENT("infinit.surface.gap.State");

namespace surface
{
  namespace gap
  {
    namespace json = elle::format::json;

    bool
    State::_wait_portal(std::string const& network_id)
    {
      ELLE_TRACE("_wait_portal for network %s", network_id);

      ELLE_DEBUG("retrieving portal path");
      auto portal_path = common::infinit::portal_path(this->_me._id, network_id);

      ELLE_DEBUG("portal path is %s", portal_path);

      if (!this->infinit_instance_manager().has_network(network_id))
        this->infinit_instance_manager().launch_network(network_id);

      for (int i = 0; i < 45; ++i)
        {
          ELLE_DEBUG("Waiting for portal.");
          if (elle::os::path::exists(portal_path))
            return true;
          if (!this->infinit_instance_manager().has_network(network_id))
            throw Exception{gap_error, "Infinit instance has crashed"};
          ::sleep(1);
        }
      return false;
    }

    InfinitInstanceManager&
    State::infinit_instance_manager()
    {
      if (!_infinit_instance_manager)
        {
          if (!this->logged_in())
            throw elle::Exception{
                "Cannot use an infinit instances manager if not logged in"
            };
          _infinit_instance_manager.reset(new InfinitInstanceManager{
              this->_me._id
          });
        }
      return *_infinit_instance_manager;
    }

    std::string
    State::create_network(std::string const& name)
    {
      ELLE_TRACE("creating network %s", name);

      metrics::google::server().store("network:create:attempt");

      plasma::meta::CreateNetworkResponse response;
      try
      {
        response = this->_meta->create_network(name);
      }
      CATCH_FAILURE_TO_METRICS("network:create");
      metrics::google::server().store("network:create:succeed");

      this->_networks_dirty = true; // XXX insert response in _networks

      this->_meta->network_add_device(
          response.created_network_id,
          this->device_id()
      );
      this->prepare_network(response.created_network_id);
      this->infinit_instance_manager().launch_network(
          response.created_network_id
      );
      this->_wait_portal(response.created_network_id);

      this->_wait_portal(response.created_network_id);
      return response.created_network_id;
    }

    void
    State::prepare_network(std::string const& network_id)
    {
      std::string const network_dir = common::infinit::network_directory(
          this->_me._id,
          network_id
      );
      if (!elle::os::path::exists(network_dir))
        elle::os::path::make_path(network_dir);

      std::string const description_filename = elle::os::path::join(
          network_dir,
          network_id + ".dsc"
      );

      if (!elle::os::path::exists(description_filename))
        {
          if (!this->network(network_id).descriptor.size())
            this->_create_network_root_block(network_id);
           this->_prepare_network_directory(network_id);
        }
     // this->_register_device();
    }

    void
    State::_create_network_root_block(std::string const& id)
    {
      ELLE_DEBUG("Creating the network descriptor.");
      // XXX this value depends on the network policy and openness.
      static nucleus::neutron::Permissions permissions =
        nucleus::neutron::permissions::read;

      auto e              = elle::Status::Error;
      auto genreDirectory = nucleus::neutron::Genre::directory;

      ELLE_DEBUG("Create proton network from id '%s'.", id);
      nucleus::proton::Network network(id);

      //- identity ----------------------------------------------------------------
      lune::Identity                identity;
      if (identity.Restore(this->_meta->identity())             == e)
        throw std::runtime_error("Couldn't restore the identity.");

      //- group -------------------------------------------------------------------
      nucleus::neutron::Group group(network, identity.pair().K(), "everybody");
      group.seal(identity.pair().k());

      //- group address -----------------------------------------------------------
      nucleus::proton::Address      group_address(group.bind());

      //- subject -----------------------------------------------------------------
      nucleus::neutron::Subject subject;
      if (subject.Create(group_address) == elle::Status::Error)
        throw std::runtime_error("unable to create the group subject");

      //- access-------------------------------------------------------------------
      nucleus::proton::Porcupine<nucleus::neutron::Access> access_porcupine{
        nucleus::proton::nest::none()};

      nucleus::proton::Door<nucleus::neutron::Access> access_door =
        access_porcupine.lookup(subject);

      access_door.open();

      access_door().insert(new nucleus::neutron::Record{subject, permissions});

      access_door.close();

      access_porcupine.update(subject);

    // XXX[cf: etoile/automaton/Access.hh>, until no longer encrypted]
#define ACCESS_SECRET_KEY_LENGTH 256
#define ACCESS_SECRET_KEY "no-secret-key"

      // XXX
      static cryptography::SecretKey secret_key(cryptography::cipher::Algorithm::aes256,
                                                ACCESS_SECRET_KEY);

      ELLE_ASSERT(access_porcupine.strategy() == nucleus::proton::Strategy::value);

      cryptography::Digest access_fingerprint =
        nucleus::neutron::access::fingerprint(access_porcupine);

      nucleus::proton::Radix access_radix = access_porcupine.seal(secret_key);

      //- directory ---------------------------------------------------------------
      nucleus::neutron::Object directory{
          network,
          identity.pair().K(),
          genreDirectory
      };

      if (directory.Update(directory.author(),
                           directory.contents(),
                           directory.size(),
                           access_radix,
                           directory.owner_token()) == e)
        throw std::runtime_error("unable to update the directory");

      if (directory.Seal(identity.pair().k(), access_fingerprint) == e)
        throw std::runtime_error("Cannot seal the access");

      //- directory address -------------------------------------------------------
      nucleus::proton::Address      directory_address(directory.bind());

      {
        elle::io::Unique root_block_;
        elle::serialize::to_string<elle::serialize::OutputBase64Archive>(
          root_block_) << directory;

        elle::io::Unique root_address_;
        elle::serialize::to_string<elle::serialize::OutputBase64Archive>(
          root_address_) << directory_address;

        elle::io::Unique group_block_;
        elle::serialize::to_string<elle::serialize::OutputBase64Archive>(
          group_block_) << group;

        elle::io::Unique group_address_;
        elle::serialize::to_string<elle::serialize::OutputBase64Archive>(
          group_address_) << group_address;

        ELLE_DEBUG("root block %s", root_block_);
        ELLE_DEBUG("root address %s", root_address_);
        ELLE_DEBUG("group block %s", group_block_);
        ELLE_DEBUG("group address %s", group_address_);

        this->_meta->update_network(
          id,
          nullptr,
          &root_block_,
          &root_address_,
          &group_block_,
          &group_address_
        );
      }

      this->_networks_dirty = true; // XXX set root_block
    }

    void
    State::_prepare_network_directory(std::string const& network_id)
    {
      ELLE_DEBUG("Prepare network directory.");
      using elle::serialize::from_string;
      using elle::serialize::InputBase64Archive;

      auto& network = this->network(network_id);

      elle::io::Path shelter_path(lune::Lune::Shelter);
      shelter_path.Complete(
          elle::io::Piece{"%USER%", this->_me._id},
          elle::io::Piece{"%NETWORK%", network_id}
      );
      ELLE_DEBUG("Shelter path == %s", shelter_path.string());
      nucleus::proton::Network proton_network(network_id);
      hole::storage::Directory storage(proton_network, shelter_path.string());

      {
        ELLE_DEBUG("Built directory storage of %s", network_id);

        ELLE_DEBUG("root block %s", network.root_block);
        ELLE_DEBUG("descriptor %s", network.descriptor);

        ELLE_ASSERT(network.root_block.size());
        ELLE_ASSERT(network.descriptor.size());

        ELLE_DEBUG("Create lune descriptor of %s", network_id);

        lune::Descriptor descriptor{
          from_string<InputBase64Archive>(network.descriptor)
        };
        ELLE_DEBUG("Lune descriptor created");

      // XXX[pas forcement necessaire si le format n'a pas change entre
      //     la version du descriptor et celle d'Infinit. il faudrait
      //     comparer static format avec ceux de reference dans le descriptor]
      //if (descriptor.version() > Infinit::version)
      //  {
      //    throw std::runtime_error("you have to update Infinit");
      //  }

      // XXX[ici plutot compare static/dynamic format car on peut passer
      //     a une nouvelle version sans que le descriptor n'ait change
      //     de format]
      //if (description.version() < Infinit::version && je suis owner)
      //  {
      //     static_assert(false, "migrate the descriptor here and send to meta");
      //  }

        lune::Identity identity;
        identity.Restore(this->_meta->identity());

        ELLE_DEBUG("Storing the descriptor of %s for user %s",
            network_id, this->_me._id);
        descriptor.store(identity);

        nucleus::neutron::Object directory{
          from_string<InputBase64Archive>(network.root_block)
        };

        storage.store(descriptor.meta().root(), directory);
        ELLE_DEBUG("Root block stored.");
      }

      {
        ELLE_DEBUG("Storing group block.");
        ELLE_DEBUG("block: '%s'.", network.group_block);
        nucleus::neutron::Group group{
          from_string<InputBase64Archive>(network.group_block)
        };
        ELLE_DEBUG("address: '%s'.", network.group_address);
        nucleus::proton::Address group_address{
          from_string<InputBase64Archive>(network.group_address)
        };
        ELLE_DEBUG("Deserialization complete.");
        storage.store(group_address, group);
        ELLE_DEBUG("Group block stored.");
      }

      ELLE_DEBUG("End of prepare directory");
    }

    std::string
    State::delete_network(std::string const& network_id, bool force)
    {
      ELLE_TRACE("deleting network %s", network_id);

      this->_networks[network_id].reset();

      metrics::google::server().store("network:delete:attempt");

      plasma::meta::DeleteNetworkResponse response;
      try
      {
        response = this->_meta->delete_network(network_id, force);
      }
      CATCH_FAILURE_TO_METRICS("network:delete");

      metrics::google::server().store("network:delete:succeed");

      this->_networks_dirty = true; // XXX remove from _networks instead
      if (this->infinit_instance_manager().has_network(response.deleted_network_id))
        {
          this->infinit_instance_manager().stop_network(
              response.deleted_network_id
          );
        }

      std::string network_path = common::infinit::network_directory(
        this->_me._id, network_id);

      if (elle::os::path::exists(network_path))
        elle::os::path::remove_directory(network_path);

      return response.deleted_network_id;
    }

    std::map<std::string, State::NetworkPtr> const&
    State::networks()
    {
      if (this->_networks_dirty)
        {
          auto response = this->_meta->networks();
          for (auto const& network_id: response.networks)
            {
              auto response = this->_meta->network(network_id);
              this->_networks[network_id].reset(new Network{response});
            }
          this->_networks_dirty = false;
        }
      return this->_networks;
    }

    Network&
    State::network(std::string const& id)
    {
      auto it = this->networks().find(id);
      if (it == this->networks().end() || it->second == nullptr)
        {
          try
            {
              auto response = this->_meta->network(id);
              this->_networks[id].reset(new Network{response});
              return *(this->_networks[id]);
            }
          catch (std::runtime_error const& e)
            {
              throw Exception{gap_network_not_found, e.what()};
            }
        }
      return *(it->second);
    }

    void
    State::network_add_user(std::string const& network_id,
                            std::string const& user)
    {
      ELLE_TRACE("adding user '%s' to network '%s'", user, network_id);

      metrics::google::server().store("network:user:add:attempt");
      try
      {
        // Retrieve user, ensuring he is on the user list.
        std::string user_id = this->user(user)._id;

        ELLE_DEBUG("adding user '%s'", user);

        Network& network = this->network(network_id);

        ELLE_DEBUG("locating 8 group");
        std::string const& group_binary = common::infinit::binary_path("8group");

        QStringList arguments;
        arguments << "--user" << this->_me._id.c_str()
                  << "--type" << "user"
                  << "--add"
                  << "--network" << network._id.c_str()
                  << "--identity" << this->user(user_id).public_key.c_str()
          ;
        ELLE_DEBUG("LAUNCH: %s %s",
                   group_binary,
                   arguments.join(" ").toStdString());
        QProcess p;
        p.start(group_binary.c_str(), arguments);
        if (!p.waitForFinished())
          throw Exception(gap_internal_error, "8group binary failed");
        if (p.exitCode())
          throw Exception(gap_internal_error, "8group binary exited with errors");

        ELLE_DEBUG("set user in network in meta.");

        auto res = this->_meta->network_add_user(network_id, user_id);
        if (std::find(network.users.begin(),
                      network.users.end(),
                      user_id) == network.users.end())
          network.users.push_back(user_id);
      }
      CATCH_FAILURE_TO_METRICS("network:user:add");

      metrics::google::server().store("network:user:add:succeed");;
    }

    //std::map<std::string, NetworkStatus*> const&
    //State::networks_status()
    //{
    //  return this->_networks_status;
    //}

    //NetworkStatus const&
    //State::network_status(std::string const& id)
    //{
    //  auto it = this->networks_status().find(id);
    //  if (it == this->networks_status().end())
    //    throw Exception{
    //        gap_error,
    //        "Unknown network id '" + id + "'",
    //    };
    //  return *(it->second);
    //}


    void
    State::network_update_callback(NetworkUpdateNotificationCallback const& cb)
    {
      auto fn = [cb] (Notification const& notif, bool) -> void {
        return cb(static_cast<NetworkUpdateNotification const&>(notif));
      };

      this->_notification_handlers[NotificationType::network_update].push_back(fn);
    }

    void
    State::_on_network_update(NetworkUpdateNotification const& notif)
    {
      ELLE_TRACE("network %s updated %s", notif.network_id, notif.what);
      // XXX do something
    }

  }
}
