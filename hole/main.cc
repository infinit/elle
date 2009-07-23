#include <QCoreApplication>

#include "hole/DHT.hh"

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  hole::DHT dht;

  dht.Port(4242);
  return app.exec();
}
