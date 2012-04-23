//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphael Londeix   [Sun 04 Mar 2012 11:40:29 AM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include <stdexcept>
#include <iostream>

#include <QDir>

#include "plasma/common/resources.hh"

#include "InfinitNetwork.hh"
#include "Manager.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

InfinitNetwork::InfinitNetwork(Manager& manager,
                               meta::NetworkResponse const& response) :
  QObject(),
  _description(response),
  _manager(manager),
  _process(),
  _infinitHome(),
  _home()
{
  std::cout << "Creating new network: " << this->_description.name
            << " (" << this->_description._id << ").\n";

  this->_infinitHome = QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY);
  this->_home = QDir(this->_infinitHome.filePath(
      QString("networks/") + this->_description.name.c_str()
  ));
  //if (!this->_process.start(homeDirectory.filePath()))
  this->_Update();
}

//
// ---------- methods  --------------------------------------------------------
//

void InfinitNetwork::Update(meta::NetworkResponse const& response)
{
  this->_description = response;
  std::cout << "Updating network: " << response._id << "\n";
  this->_Update();
}


/// XXX should be in a library

#include "lune/Descriptor.hh"
#include "lune/Identity.hh"
#include "lune/Passport.hh"
#include "lune/Set.hh"
#include "nucleus/neutron/Object.hh"
#include "nucleus/neutron/Genre.hh"
#include "nucleus/neutron/Access.hh"
#include "nucleus/proton/Address.hh"

#define LOG() std::cerr << "{WTG} " << "InfinitNetwork::" << __FUNCTION__ << "(id=" \
                        << this->_description._id << "): "

void InfinitNetwork::_Update()
{
  LOG() << "Starting infinit network update\n";
  if (!this->_home.exists() && !this->_home.mkpath("."))
      throw std::runtime_error("Cannot create network home");
  QString descriptionFilename = this->_description.name.c_str() + QString(".dsc");
  if (!this->_home.exists(descriptionFilename))
    {
      if (!this->_description.descriptor.size())
        return this->_CreateNetworkRootBlock();
      else
        {
          std::cerr << "Already has a nice descriptor : " << this->_description.descriptor << "\n";
          this->_PrepareDirectory();
        }
    }
  else
    this->_RegisterDevice();
}

/// Called when the network does not have any descriptor
void InfinitNetwork::_CreateNetworkRootBlock()
{
  LOG() << "Creating the network descriptor\n";
  lune::Identity                identity;
  nucleus::neutron::Object      directory;
  nucleus::proton::Address      address;

  auto e              = elle::Status::Error;
  auto genreDirectory = nucleus::neutron::GenreDirectory;
  auto access         = nucleus::neutron::Access::Null;

  if (identity.Restore(this->_manager.identity())             == e ||
      directory.Create(genreDirectory, identity.pair.K)       == e ||
      directory.Seal(identity.pair.k, access)                 == e ||
      directory.Bind(address)                                 == e)
    {
      throw std::runtime_error("Couldn't create the root block");
    }

  elle::Unique              rootBlock;
  elle::Unique              rootAddress;

  directory.Save(rootBlock);
  address.Save(rootAddress);

  this->_manager.meta().UpdateNetwork(
      this->_description._id,
      nullptr,
      nullptr,
      nullptr,
      &rootBlock,
      &rootAddress,
      boost::bind(&InfinitNetwork::_OnGotDescriptor, this, _1),
      boost::bind(&InfinitNetwork::_OnAnyError, this, _1, _2)
  );
}

/// Prepare the network directory, store root block and network descriptor
void InfinitNetwork::_PrepareDirectory()
{
  LOG() << "Dumping root block\n";

  assert(this->_description.root_block.size());
  assert(this->_description.descriptor.size());

  lune::Descriptor descriptor;

  auto e = elle::Status::Error;
  if (descriptor.Restore(this->_description.descriptor) == e ||
      descriptor.Store(this->_description.name)          == e)
    {
      show();
      throw std::runtime_error("Couldn't save the descriptor");
    }

  nucleus::proton::Network  network;
  nucleus::neutron::Object directory;

  if (network.Create(this->_description.name)          == e ||
      directory.Restore(this->_description.root_block) == e ||
      directory.Store(network, descriptor.root)        == e)
    {
      show();
      throw std::runtime_error("Couldn't store the root block");
    }

  LOG() << "Root block stored\n";

  this->_RegisterDevice();
}


/// Append the local device to the network
void InfinitNetwork::_RegisterDevice()
{
  LOG() << "Check if the device is registered for this network\n";
  lune::Passport passport;

  if (passport.Load() == elle::Status::Error)
    {
      show();
      throw std::runtime_error("Couldn't load the passport file :'(");
    }

  auto it = std::find(
      this->_description.devices.begin(),
      this->_description.devices.end(),
      passport.id
  );

  if (it == this->_description.devices.end())
    {
      LOG() << "Registering device for this network\n";

      this->_description.devices.push_back(passport.id);
      this->_manager.meta().UpdateNetwork(
          this->_description._id,
          nullptr,
          nullptr,
          &this->_description.devices,
          nullptr,
          nullptr,
          boost::bind(&InfinitNetwork::_OnDeviceRegistered, this, _1),
          boost::bind(&InfinitNetwork::_OnAnyError, this, _1, _2)
      );
    }
  else
    {
      this->_manager.meta().GetNetworkNodes(
          this->_description._id,
          boost::bind(&InfinitNetwork::_OnNetworkNodes, this, _1),
          boost::bind(&InfinitNetwork::_OnAnyError, this, _1, _2)
      );
    }
}

void InfinitNetwork::_OnDeviceRegistered(meta::UpdateNetworkResponse const& response)
{
  LOG() << "Device successfully registered\n";
  assert(response.updated_network_id == this->_description._id);
  this->_manager.meta().GetNetworkNodes(
      this->_description._id,
      boost::bind(&InfinitNetwork::_OnNetworkNodes, this, _1),
      boost::bind(&InfinitNetwork::_OnAnyError, this, _1, _2)
  );
}

/// Update the network nodes set when everything is good
void InfinitNetwork::_OnNetworkNodes(meta::NetworkNodesResponse const& response)
{
  LOG() << "Got network nodes :\n";
  lune::Set locusSet;
  auto it =  response.nodes.begin(),
       end = response.nodes.end();
  for (; it != end; ++it)
    {
      LOG() << "\t\t * " << *it << "\n";
      elle::network::Locus locus;

      if (locus.Create(*it) == elle::Status::Error)
        throw std::runtime_error("Cannot create locus from string '" + *it + "'");
      if (locusSet.Add(locus) == elle::Status::Error)
        {
          LOG() << "Cannot add locus '" << *it << "' to the set (ignored)\n";
        }
    }

  if (locusSet.Store(this->_description.name) == elle::Status::Error)
    throw std::runtime_error("Cannot store the locus set");
  this->_StartProcess();

}
void InfinitNetwork::_OnGotDescriptor(meta::UpdateNetworkResponse const& response)
{
  LOG() << "Got network descriptor\n";
  // XXX updated is none here, correct meta
  if (response.updated_network_id != this->_description._id)
    {
      throw std::runtime_error(
          "mismatch ids... between updated '" +
          response.updated_network_id + "' and the old one '" +
          this->_description._id + "'"
      );
    }
  std::cout << "Got descriptor for " << this->_description.name
            <<  " (" << this->_description._id << ") :"
            << response.descriptor << std::endl;

  this->_description.root_block = response.root_block;
  this->_description.descriptor = response.descriptor;

  this->_PrepareDirectory();
}

void InfinitNetwork::_OnAnyError(meta::MetaClient::Error error, std::string const& err)
{
  LOG() << "Got error while creating the network '" << this->_description.name
        << ": " << err << " (" <<(int)error<<").\n";
}

void InfinitNetwork::_StartProcess()
{
  LOG() << "Starting process\n";
  QDir mnt(
      this->_infinitHome.filePath(
          QString::fromStdString(
            "networks/" + this->_description.name + "/mnt"
          )
      )
  );
  if (!mnt.exists() && !mnt.mkpath("."))
    throw std::runtime_error(
        "Cannot create the mount directory '" + mnt.path().toStdString() + "'"
    );

  QStringList arguments;
  arguments << "-n" << this->_description.name.c_str()
            << "-m" << mnt.path();

  this->_process.start(
      this->_infinitHome.filePath("bin/8infinit"),
      arguments
  );

  this->connect(
      &this->_process, SIGNAL(started()),
      this, SLOT(_OnProcessStarted())
  );

  this->connect(
      &this->_process, SIGNAL(error(QProcess::ProcessError)),
      this, SLOT(_OnProcessError(QProcess::ProcessError))
  );
}



//
// ---------- slots ------------------------------------------------------------
//

void InfinitNetwork::_OnProcessStarted()
{
  LOG() << "Process successfully started!\n";
}

void InfinitNetwork::_OnProcessError(QProcess::ProcessError error)
{
  LOG() << "Process has an error\n";
}
