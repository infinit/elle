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


void InfinitNetwork::_OnGotDescriptor(meta::UpdateNetworkResponse const& response)
{
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

  this->_RegisterDevice();
}

void InfinitNetwork::_OnAnyError(meta::MetaClient::Error error, std::string const& err)
{
  std::cerr << "Got error while creating the network '" << this->_description.name
            << "' (" << this->_description._id << ")"
            << ": " << err << " (" <<(int)error<<").\n";
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

void InfinitNetwork::_CreateNetworkRootBlock()
{
  lune::Identity                identity;
  nucleus::neutron::Object      directory;
  nucleus::proton::Address      address;

  auto e              = elle::StatusError;
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

  using namespace std::placeholders;
  this->_manager.meta().UpdateNetwork(
      this->_description._id,
      nullptr,
      nullptr,
      nullptr,
      &rootBlock,
      &rootAddress,
      std::bind(&InfinitNetwork::_OnGotDescriptor, this, _1),
      std::bind(&InfinitNetwork::_OnAnyError, this, _1, _2)
  );
}

void InfinitNetwork::_PrepareDirectory()
{

  assert(this->_description.root_block.size());
  assert(this->_description.descriptor.size());

  lune::Descriptor descriptor;

  auto e = elle::StatusError;
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

  std::cerr << "Root block stored\n";
}

void InfinitNetwork::_Update()
{
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

  using namespace std::placeholders;
  this->_manager.meta().GetNetworkNodes(
      this->_description._id,
      std::bind(&InfinitNetwork::_OnNetworkNodes, this, _1)
  );

}


void InfinitNetwork::_RegisterDevice()
{
  lune::Passport passport;

  if (passport.Load() == elle::StatusError)
    throw std::runtime_error("Couldn't load the passport file :'(");

  auto it = std::find(
      this->_description.devices.begin(),
      this->_description.devices.end(),
      passport.id
  );

  if (it == this->_description.devices.end())
    {
      using namespace std::placeholders;

      this->_description.devices.push_back(passport.id);
      this->_manager.meta().UpdateNetwork(
          this->_description._id,
          nullptr,
          nullptr,
          &this->_description.devices,
          nullptr,
          nullptr,
          std::bind(&InfinitNetwork::_OnDeviceRegistered, this, _1),
          std::bind(&InfinitNetwork::_OnAnyError, this, _1, _2)
      );
    }
  else
    this->_PrepareDirectory();
}

void InfinitNetwork::_OnDeviceRegistered(meta::UpdateNetworkResponse const& response)
{
  this->_PrepareDirectory();
}

void InfinitNetwork::_OnNetworkNodes(meta::NetworkNodesResponse const& response)
{
  lune::Set locusSet;
  auto it =  response.nodes.begin(),
       end = response.nodes.end();
  for (; it != end; ++it)
    {
      elle::network::Locus locus;

      locus.Create(*it);
      locusSet.Add(locus); // XXX
    }

  locusSet.Store(this->_description.name);
}
