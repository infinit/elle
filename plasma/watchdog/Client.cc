//
// ---------- header ----------------------------------------------------------
//
// project       @PROJECT@
//
// license       infinit
//
// author        @NAME@   [Fri 02 Mar 2012 02:30:28 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include <iostream>

#include "Client.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

Client::Client(QApplication& app) : _api(app) {}
Client::~Client() { std::cerr << "Client::~Client()\n"; }

//
// ---------- methods  --------------------------------------------------------
//

void Client::Update(std::function<void()> callback)
{
  using namespace std::placeholders;
  this->_api.GetNetworks(
      std::bind(&Client::_OnNetworkList, this, callback, _1)
  );
}

void Client::_OnNetworkList(std::function<void()> callback,
                            NetworksResponse const& response)
{
  std::cout << "Network response :\n";
  auto it = response.networks.begin(), end = response.networks.end();
  for (; it != end; ++it)
    std::cout << "NetworkId: " << *it << "\n";
  std::cout << std::flush;
  callback();
}
