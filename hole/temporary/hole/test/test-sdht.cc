#include <elle/misc/Report.hh>
#include <hole/sdht/Sdht.hh>

#include <iostream>
#include <string>

using namespace elle;
using namespace elle::misc;

using namespace hole;
using namespace hole::sdht;




int		create(int	port)
{
  Sdht		*dht = new Sdht("127.0.0.1", port);
  
  dht->create();
  dht->print();
  
  while (true)
    sleep(10000);
  
  delete dht;
  
  return 0;
}

int		join(int	port,
		     char	*rip,
		     int	rport)
{
  Sdht		*dht = new Sdht("127.0.0.1", port);
  
  dht->join(rip, rport);
  dht->print();
  
  dht->lookup(Key());

  while (true)
    sleep(10000);
  
  delete dht;
  return 0;
}


int		main(int	argc,
		     char	**argv)
{
  if (argc == 2)
    return (create(atoi(argv[1])));
  if (argc == 4)
    return (join(atoi(argv[1]), argv[2], atoi(argv[3])));
  
  std::cerr << "Bad usage" << std::endl;
  return 1;
}
