#include <stdlib.h>
#include <QCoreApplication>

#include "hole/DHT.hh"
#include "hole/DHTRequest.hh"

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  hole::DHT dht;

  /* Checking for specified local port */
  char * HOLE_PORT = getenv("HOLE_PORT");
  if (HOLE_PORT)
    dht.Port(strtol(HOLE_PORT, 0, 0));
  else
    dht.Port(0);

  /* Do we join a network ? */
  char * HOLE_PEER_ADDR = getenv("HOLE_PEER_ADDR");
  char * HOLE_PEER_PORT = getenv("HOLE_PEER_PORT");
  if (HOLE_PEER_ADDR && HOLE_PEER_PORT)
  {
    hole::DHTJoinRequest * jrq = new hole::DHTJoinRequest;
    jrq->address = QHostAddress(HOLE_PEER_ADDR);
    jrq->port = strtol(HOLE_PEER_PORT, 0, 0);
    dht.Join(jrq);
  }
  else
    dht.Create();

  return app.exec();
}
