#include "hole/DHT.hh"
#include "hole/DHTRequestHandler.hh"

namespace hole
{
  DHTRequestHandler::DHTRequestHandler(DHT & dht)
    : QObject(&dht),
      dht_(dht),
      tag_()
  {
  }
}
