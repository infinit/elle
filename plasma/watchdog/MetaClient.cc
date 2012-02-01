//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphael Londeix   [Wed 01 Feb 2012 07:15:38 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include "plasma/common/resources.hh"

#include "MetaClient.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

MetaClient::MetaClient(std::string const& server_address) :
  _server_address(server_address),
  _network_access_manager()
{}

MetaClient::~MetaClient()
{}

//
// ---------- methods  --------------------------------------------------------
//


void MetaClient::GetNetworks(NetworksCallback callback)
{
}
