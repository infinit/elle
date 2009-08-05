#include "hole/DHT.hh"
#include "hole/DHTRequest.hh"

namespace hole
{
  DHTRequest::DHTRequest(DHT & dht)
    : QObject(&dht),
      dht_(dht),
      tag_()
  {
  }

  DHTRequest::~DHTRequest()
  {
  }
}
