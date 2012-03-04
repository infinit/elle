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

#include <iostream>

#include <QDir>

#include "plasma/common/resources.hh"

#include "InfinitNetwork.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

InfinitNetwork::InfinitNetwork(meta::NetworkResponse const& response) :
  _description(response),
  _process()
{
  std::cout << "Creating new network: " << this->_description.name
            << " (" << this->_description._id << ").\n";

  QDir homeDirectory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));
  //if (!this->_process.start(homeDirectory.filePath()))
}

//
// ---------- methods  --------------------------------------------------------
//

void InfinitNetwork::Update(meta::NetworkResponse const& response)
{
  this->_description = response;
  std::cout << "Updating network: " << response._id << "\n";
}

