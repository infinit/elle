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
      QString("networks") + this->_description.name.c_str()
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


void InfinitNetwork::_Update()
{
  if (!this->_home.exists() && !this->_home.mkpath("."))
      throw std::runtime_error("Cannot create network home");
  QString descriptionFilename = this->_description.name.c_str() + QString(".dsc");
  if (!this->_home.exists(descriptionFilename))
    {
      if (!this->_description.descriptor.size())
        return this->_CreateNetworkRootBlock();
      this->_PrepareDirectory();
    }
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
  this->_PrepareDirectory();
}

void InfinitNetwork::_OnGotDescriptorError(meta::MetaClient::Error error, std::string const& err)
{
  std::cerr << "Got error while getting network descriptor: " << err << ".\n";
}




/// XXX should be in a library

#include "lune/Descriptor.hh"
#include "lune/Identity.hh"
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

  elle::Unique rootBlock;
  elle::Unique rootAddress;

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
      std::bind(&InfinitNetwork::_OnGotDescriptorError, this, _1, _2)
  );
}

void InfinitNetwork::_PrepareDirectory()
{
  nucleus::neutron::Object directory;
  nucleus::proton::Address address;

  lune::Descriptor descriptor;

  auto e = elle::StatusError;
  if (descriptor.Restore(this->_description.descriptor) == e ||
      descriptor.Save(this->_description.name)          == e)
    {
      throw std::runtime_error("Couldn't save the descriptor");
    }
}


