#include <stdexcept>
#include <iostream>

#include <QDir>

#include <common/common.hh>

#include <elle/log.hh>
#include <elle/os/path.hh>

#include <lune/Descriptor.hh>
#include <lune/Identity.hh>
#include <lune/Passport.hh>
#include <lune/Set.hh>

#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Genre.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Trait.hh>
#include <nucleus/proton/Address.hh>

#include <elle/idiom/Close.hh>

#include "InfinitNetwork.hh"
#include "Manager.hh"

#define LOG(...)                                                              \
  elle::log::debug("InfinitNetwork::", __FUNCTION__,                          \
                   "(id=", this->_description._id, "):", __VA_ARGS__)         \
  /**/

using namespace plasma::watchdog;

InfinitNetwork::InfinitNetwork(Manager& manager,
                               meta::NetworkResponse const& response)
  : QObject()
  , _description(response)
  , _manager(manager)
  , _process()
  , _mount_point{
      elle::os::path::join(
        common::infinit::home(),
        "networks",
        _description.name,
        "mnt").c_str()
  }
{
  LOG("Creating new network.");
  this->_update();
}

InfinitNetwork::~InfinitNetwork()
{
  if (this->_process.state() != QProcess::NotRunning)
    {
      elle::log::warn("Network ", this->_description.name, "not terminated !");
    }
}

std::string InfinitNetwork::mount_point() const
{
  return this->_mount_point.path().toStdString();
}

std::string const& InfinitNetwork::id() const
{
  return this->_description._id;
}

void InfinitNetwork::update(meta::NetworkResponse const& response)
{
  LOG("Updating network:", response._id);
  this->_description = response;
  this->_update();
}

void InfinitNetwork::stop()
{
  elle::log::debug("Shutting down network", this->_description.name);
  this->_process.terminate();
  //this->_process.waitForFinished();
  elle::log::debug("Done!");
}

void InfinitNetwork::_update()
{
  LOG("Starting network update.");
  QDir home{
    elle::os::path::join(
      common::infinit::home(),
      "networks",
      this->_description.name).c_str()
  };
  if (!home.exists() && !home.mkpath("."))
      throw std::runtime_error("Cannot create network home");

  QString descriptionFilename =
    this->_description.name.c_str() + QString(".dsc");

  if (!home.exists(descriptionFilename))
    {
      if (!this->_description.descriptor.size())
        return this->_create_network_root_block();
      else
        {
          LOG("Already has a nice descriptor:",
              this->_description.descriptor);
          this->_prepare_directory();
        }
    }
  else
    this->_register_device();
}

/// Called when the network does not have any descriptor
void InfinitNetwork::_create_network_root_block()
{
  LOG("Creating the network descriptor.");
  // XXX this value depends on the network policy and openness.
  static nucleus::neutron::Permissions permissions =
    nucleus::neutron::PermissionRead;

  auto e              = elle::Status::Error;
  auto genreDirectory = nucleus::neutron::GenreDirectory;

  //- identity ----------------------------------------------------------------
  lune::Identity                identity;
  if (identity.Restore(this->_manager.identity())             == e)
    throw std::runtime_error("Couldn't restore the identity.");

  //- group -------------------------------------------------------------------
  nucleus::neutron::Group group(identity.pair.K, "everybody");
  group.seal(identity.pair.k);

  //- group address -----------------------------------------------------------
  nucleus::proton::Address      group_address;
  if (group.Bind(group_address) == elle::Status::Error)
    throw std::runtime_error("unable to bind the group");

  //- subject -----------------------------------------------------------------
  nucleus::neutron::Subject subject;
  if (subject.Create(group_address) == elle::Status::Error)
    throw std::runtime_error("unable to create the group subject");

  //- access-------------------------------------------------------------------
  nucleus::neutron::Access access;
  if (access.Add(new nucleus::neutron::Record{
        subject,
        permissions,
        nucleus::neutron::Token::Null
        }) == elle::Status::Error)
    throw std::runtime_error("unable to add the record to the access");

  //- access address ----------------------------------------------------------
  nucleus::proton::Address      access_address;
  if (access.Bind(access_address) == elle::Status::Error)
    throw std::runtime_error("unable to bind the access");

  //- directory ---------------------------------------------------------------
  nucleus::neutron::Object      directory;
  if (directory.Create(genreDirectory, identity.pair.K)       == e)
    throw std::runtime_error("Couldn't create the root block.");

  if (directory.Update(directory.author(),
                       directory.contents(),
                       directory.size(),
                       access_address,
                       directory.owner_token()) == e)
    throw std::runtime_error("unable to update the directory");

  if (directory.Seal(identity.pair.k, access)                 == e)
    throw std::runtime_error("Cannot seal the access");

  //- directory address -------------------------------------------------------
  nucleus::proton::Address      directory_address;
  if (directory.Bind(directory_address)                       == e)
    throw std::runtime_error("Couldn't bind the address.");

  {
    elle::io::Unique root_block_;
    directory.Save(root_block_);
    elle::io::Unique root_address_;
    directory_address.Save(root_address_);

    elle::io::Unique access_block_;
    access.Save(access_block_);
    elle::io::Unique access_address_;
    access_address.Save(access_address_);

    elle::io::Unique group_block_;
    group.Save(group_block_);
    elle::io::Unique group_address_;
    group_address.Save(group_address_);

    this->_on_got_descriptor(this->_manager.meta().update_network(
          this->_description._id,
          nullptr,
          nullptr,
          nullptr,
          &root_block_,
          &root_address_,
          &access_block_,
          &access_address_,
          &group_block_,
          &group_address_
    ));
  }
}

/// Prepare the network directory, store root block and network descriptor
void InfinitNetwork::_prepare_directory()
{
  LOG("Prepare network directory.");

  assert(this->_description.root_block.size());
  assert(this->_description.descriptor.size());

  lune::Descriptor descriptor;

  auto e = elle::Status::Error;
  if (descriptor.Restore(this->_description.descriptor) == e ||
      descriptor.Store(this->_description.name)         == e)
    {
#include <elle/idiom/Open.hh>
      show();
#include <elle/idiom/Close.hh>
      throw std::runtime_error("Couldn't save the descriptor.");
    }

  nucleus::proton::Network network;
  nucleus::neutron::Object directory;

  if (network.Create(this->_description.name)          == e ||
      directory.Restore(this->_description.root_block) == e ||
      directory.MutableBlock::Store(network, descriptor.root) == e)
    {
#include <elle/idiom/Open.hh>
      show();
#include <elle/idiom/Close.hh>
      throw std::runtime_error("Couldn't store the root block.");
    }

  nucleus::neutron::Access access;
  nucleus::proton::Address access_address;
  if (access.Restore(this->_description.access_block) == e ||
      access_address.Restore(this->_description.access_address) == e ||
      access.Store(network, access_address) == e)
    throw std::runtime_error("Couldn't store the access block");

  nucleus::neutron::Group group;
  nucleus::proton::Address group_address;
  if (group.Restore(this->_description.group_block) == e ||
      group_address.Restore(this->_description.group_address) == e ||
      group.Store(network, group_address) == e)
    throw std::runtime_error("Couldn't store the group block");

  LOG("Root block stored.");

  this->_register_device();
}


/// Append the local device to the network
void InfinitNetwork::_register_device()
{
  LOG("Check if the device is registered for this network.");
  lune::Passport passport;

  if (passport.Load() == elle::Status::Error)
    {
#include <elle/idiom/Open.hh>
      show();
#include <elle/idiom/Close.hh>
      throw std::runtime_error("Couldn't load the passport file :'(");
    }

  LOG("search device for the passport id:", passport.id);
  auto it = std::find(
      this->_description.devices.begin(),
      this->_description.devices.end(),
      passport.id
  );

  if (it == this->_description.devices.end())
    {
      LOG("Registering device for this network.");

      this->_description.devices.push_back(passport.id);
      this->_on_device_registered(this->_manager.meta().update_network(
          this->_description._id,
          nullptr,
          nullptr,
          &this->_description.devices,
          nullptr,
          nullptr,
          nullptr,
          nullptr,
          nullptr,
          nullptr
      ));
    }
  else
    {
      LOG("Get network nodes.");
      this->_on_network_nodes(this->_manager.meta().network_nodes(
          this->_description._id
      ));
    }
}

void InfinitNetwork::_on_device_registered(meta::UpdateNetworkResponse const& response)
{
  LOG("Device successfully registered.");
  assert(response.updated_network_id == this->_description._id);
  this->_on_network_nodes(this->_manager.meta().network_nodes(
      this->_description._id
  ));
}

/// Update the network nodes set when everything is good
void InfinitNetwork::_on_network_nodes(meta::NetworkNodesResponse const& response)
{
  LOG("Got network nodes:");
  lune::Set locusSet;
  auto it =  response.nodes.begin(),
       end = response.nodes.end();
  for (; it != end; ++it)
  {
    LOG("\t * ", *it);
    elle::network::Locus locus;

    if (locus.Create(*it) == elle::Status::Error)
      throw std::runtime_error("Cannot create locus from string '" + *it + "'.");
    if (locusSet.Add(locus) == elle::Status::Error)
      {
        LOG("Cannot add locus '", elle::iomanip::nosep, *it,
            "' to the set (ignored).");
      }
  }

  if (locusSet.Store(this->_description.name) == elle::Status::Error)
    throw std::runtime_error("Cannot store the locus set");
  this->_start_process();

}
void InfinitNetwork::_on_got_descriptor(meta::UpdateNetworkResponse const& response)
{
  LOG("Got network descriptor.");
  // XXX updated is none here, correct meta
  if (response.updated_network_id != this->_description._id)
    {
      throw std::runtime_error(
          "mismatch ids... between updated '" +
          response.updated_network_id + "' and the old one '" +
          this->_description._id + "'"
      );
    }
  this->_description = this->_manager.meta().network(this->_description._id);
  LOG("Got descriptor for", this->_description.name,
      "(", this->_description._id, ") :", this->_description.descriptor);

  this->_prepare_directory();
}

void InfinitNetwork::_on_any_error(plasma::meta::Error error, std::string const& err)
{
  LOG("Got error while creating the network '",
      elle::iomanip::nosep, this->_description.name,
      ": ", err, " (", (int)error, ").");
}

void InfinitNetwork::_start_process()
{
  if (this->_process.state() != QProcess::NotRunning)
    return;

  LOG("Starting infinit process");

  if (!this->_mount_point.exists() && !_mount_point.mkpath("."))
    throw std::runtime_error(
        "Cannot create the mount directory '" + _mount_point.path().toStdString() + "'"
    );

  QDir home_mnt{
      elle::os::path::join(common::system::home_directory(), "Infinit").c_str()
  };

  if (home_mnt.exists() || home_mnt.mkpath("."))
    {
      LOG("Created mount point", home_mnt.path().toStdString());
      auto link_path = home_mnt.filePath(this->_description.name.c_str());
#ifdef INFINIT_WINDOWS
      link_path += ".lnk";
#endif
      if (!QFile(this->_mount_point.path()).link(link_path))
        elle::log::debug("Cannot create links to mount points.");
    }
  else
    elle::log::warn("Cannot create mount point directory.");


  this->connect(
      &this->_process, SIGNAL(started()),
      this, SLOT(_on_process_started())
  );

  this->connect(
      &this->_process, SIGNAL(error(QProcess::ProcessError)),
      this, SLOT(_on_process_error(QProcess::ProcessError))
  );

  this->connect(
      &this->_process, SIGNAL(finished(int, QProcess::ExitStatus)),
      this, SLOT(_on_process_finished(int, QProcess::ExitStatus))
  );

  LOG("exec:",
      common::infinit::binary_path("8infinit"),
      "-n", this->_description.name.c_str(),
      "-m", this->_mount_point.path().toStdString(),
      "-u", this->_manager.user().c_str()
  );

  QStringList arguments;
  arguments << "-n" << this->_description.name.c_str()
            << "-m" << this->_mount_point.path()
            << "-u" << this->_manager.user().c_str()
            ;

  this->_process.start(
      common::infinit::binary_path("8infinit").c_str(),
      arguments
  );
}



//
// ---------- slots ------------------------------------------------------------
//

void InfinitNetwork::_on_process_started()
{
  LOG("Process successfully started.");
}

void InfinitNetwork::_on_process_error(QProcess::ProcessError)
{
  LOG("Process has an error.");
}

void InfinitNetwork::_on_process_finished(int exit_code, QProcess::ExitStatus)
{
  LOG("Process finished with exit code ", exit_code);
  if (true || exit_code) // XXX
    {
      auto stdout = this->_process.readAllStandardOutput();
      auto stderr = this->_process.readAllStandardError();
      std::cerr << "=================================== standard output:\n"
                << QString(stdout).toStdString()
                << "\n=================================== standard error:\n"
                << QString(stderr).toStdString()
                ;

    }
}
