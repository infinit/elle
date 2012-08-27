#include "InfinitNetwork.hh"
#include "Manager.hh"

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

#include <QDir>

#include <stdexcept>
#include <iostream>

#include <stdlib.h>
#include <sys/wait.h>


#define LOG(Fmt, ...)                                                         \
  ELLE_DEBUG("InfinitNetwork::%s(id=%s): " Fmt,                               \
             __FUNCTION__, this->_description._id, ##__VA_ARGS__);            \
  /**/

ELLE_LOG_COMPONENT("infinit.plasma.watchdog");

using namespace plasma::watchdog;
namespace path = elle::os::path;

InfinitNetwork::InfinitNetwork(Manager& manager,
                               meta::NetworkResponse const& response)
  : QObject()
  , _description(response)
  , _manager(manager)
  , _process()
  , _network_dir{path::join(common::infinit::home(), "networks", response._id)}
  , _mount_point{path::join(_network_dir, "mnt")}
{
  LOG("Creating new network.");

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
  this->_update();
}

InfinitNetwork::~InfinitNetwork()
{
  if (this->_process.state() != QProcess::NotRunning)
    {
      ELLE_WARN("Network %s not terminated !", this->_description.name);
    }
}

std::string InfinitNetwork::mount_point() const
{
  return this->_mount_point;
}

std::string const& InfinitNetwork::id() const
{
  return this->_description._id;
}

void InfinitNetwork::update(meta::NetworkResponse const& response)
{
  LOG("Updating network: %s", response._id);
  this->_description = response;
  this->_update();
}

void InfinitNetwork::stop()
{
  ELLE_DEBUG("Shutting down network %s", this->_description.name);
  this->_process.terminate();
  //this->_process.waitForFinished();
  ELLE_DEBUG("Done!");
}

void InfinitNetwork::_update()
{
  LOG("Starting network update.");
  if (!path::exists(this->_network_dir))
    path::make_path(this->_network_dir);

  std::string description_filename = path::join(
      this->_network_dir,
      this->_description._id + ".dsc"
  );

  if (!path::exists(description_filename))
    {
      if (!this->_description.descriptor.size())
        return this->_create_network_root_block();
      else
          this->_prepare_directory();
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
  // XXX descriptor.Store() usually take a network name
  if (descriptor.Restore(this->_description.descriptor) == e ||
      descriptor.Store(this->_description._id)          == e)
    {
#include <elle/idiom/Open.hh>
      show();
#include <elle/idiom/Close.hh>
      throw std::runtime_error("Couldn't save the descriptor.");
    }

  nucleus::proton::Network network;
  nucleus::neutron::Object directory;

  // XXX network.Create() usually take a network name
  if (network.Create(this->_description._id)          == e ||
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

    this->_manager.meta().network_add_device(
        this->_description._id,
        passport.id
    );
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
    LOG("\t * %s", *it);
    elle::network::Locus locus;

    if (locus.Create(*it) == elle::Status::Error)
      throw std::runtime_error("Cannot create locus from string '" + *it + "'.");
    if (locusSet.Add(locus) == elle::Status::Error)
      {
        LOG("Cannot add locus '%s' to the set (ignored).", *it);
      }
  }

  if (locusSet.Store(this->_description._id) == elle::Status::Error)
    throw std::runtime_error("Cannot store the locus set");
  this->_start_process();

}
void InfinitNetwork::_on_got_descriptor(meta::UpdateNetworkResponse const& response)
{
  LOG("Got network descriptor.");
  if (response.updated_network_id != this->_description._id)
    {
      throw std::runtime_error(
          "mismatch ids... between updated '" +
          response.updated_network_id + "' and the old one '" +
          this->_description._id + "'"
      );
    }
  this->_description = this->_manager.meta().network(this->_description._id);
  LOG("Got descriptor for %s (%s): %s", this->_description.name,
      this->_description._id, this->_description.descriptor);

  this->_prepare_directory();
}

void InfinitNetwork::_on_any_error(plasma::meta::Error error, std::string const& err)
{
  LOG("Got error while creating the network '%s': %s (%s)",
      this->_description.name, err, (int)error);
}

void InfinitNetwork::_start_process()
{
  if (this->_process.state() != QProcess::NotRunning)
    return;

  LOG("Starting infinit process (mount point: %s", this->_mount_point);

  path::make_path(this->_mount_point);

  std::string mnt_link_dir = path::join(
    common::system::home_directory(),
    "Infinit"
  );
  if (!path::exists(mnt_link_dir))
    path::make_directory(mnt_link_dir);

  std::string owner_email = this->_manager.meta().user(
      this->_description.owner
  ).email;

  std::string mnt_link = path::join(
    mnt_link_dir,
    elle::sprintf("%s (%s)", this->_description.name, owner_email)
  );

  path::make_symlink(this->_mount_point, mnt_link);

  LOG("exec: %s -n %s -m %s -u %s",
      common::infinit::binary_path("8infinit"),
      this->_description._id.c_str(),
      this->_mount_point,
      this->_manager.user().c_str());

  QStringList arguments;
  arguments << "-n" << this->_description._id.c_str()
            << "-m" << this->_mount_point.c_str()
            << "-u" << this->_manager.user().c_str()
            ;

  // XXX[rename into [network-name].log]
  std::string log_out = path::join(this->_network_dir, "out.log").c_str();
  std::string log_err = path::join(this->_network_dir, "err.log").c_str();
  std::string pid_file = path::join(this->_network_dir, "run.pid").c_str();

  if (elle::os::path::exists(pid_file))
    {
      pid_t pid = 0;

        {
          std::ifstream in(pid_file);
          if (in.good())
            in >> pid;
          in.close();
        }

      if (pid != 0)
        {
          if (kill(pid, SIGINT) == 0)
            {
              (void)kill(pid, SIGKILL);
            }
          sleep(1);
          char const* const umount_arguments[] = {
#ifdef INFINIT_MACOSX
              "umount",
              this->_mount_point.c_str(),
#endif
#ifdef INFINIT_LINUX
              "fusermount"
              "-z", "-u",
              this->_mount_point.c_str(),
#endif
              nullptr
          };
          pid_t umount_pid = fork();
          if (umount_pid == 0)
            {
              close(STDERR_FILENO);
              if (execvp(umount_arguments[0], (char**) umount_arguments) != 0)
                {
                  ELLE_ERR("Cannot launch %s", umount_arguments[0]);
                }
              exit(1);
            }
          else if (umount_pid > 0)
            {
              int status;
              (void) waitpid(umount_pid, &status, 0);
            }
        }

        {
          std::ofstream out(pid_file);
          if (out.good())
            out << 0;
          out.close();
        }
    }

  this->_process.setStandardOutputFile(log_out.c_str());
  this->_process.setStandardErrorFile(log_err.c_str());

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
  std::string pid_file = elle::os::path::join(_network_dir, "run.pid").c_str();
  LOG("Process successfully started (pid = %s)", this->_process.pid());
    {
      std::ofstream out(pid_file);
      if (out.good())
        out << this->_process.pid();
    }
}

void InfinitNetwork::_on_process_error(QProcess::ProcessError)
{
  ELLE_ERR("Process has an error.");
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

void InfinitNetwork::_on_process_finished(int exit_code, QProcess::ExitStatus)
{
  ELLE_ERR("Process finished with exit code %s", exit_code);
  if (exit_code != 0)
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
