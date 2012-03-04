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

Client::Client(MetaClient& meta) : _meta(meta) {}
Client::~Client() { std::cerr << "Client::~Client()\n"; }

//
// ---------- methods  --------------------------------------------------------
//
