#include <iostream>

#include <elle/printf.hh>
#include <elle/network/Interface.hh>

// FIXME: How is this a "test" ??
int
main()
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
  std::cout << "tests done." << std::endl;
  return (0);
}
