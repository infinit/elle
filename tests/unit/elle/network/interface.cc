#include <elle/network/Interface.hh>
#include <iostream>

int
main(int argc, const char *argv[])
{
  auto interfaces = elle::network::Interface::get_map(
      elle::network::Interface::Filter::only_up
      | elle::network::Interface::Filter::no_loopback
  );
  for (auto const& pair: interfaces)
  {
      std::cout << "IP: " << pair.second.ipv4_address << std::endl
                << "MAC: " << pair.second.mac_address << std::endl;
  }
  return (0);
}
