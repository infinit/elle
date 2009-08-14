//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/main.cc
//
// created       julien quintard   [fri jul 31 21:49:34 2009]
// updated       julien quintard   [fri aug 14 12:45:41 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <pig/PIG.hh>

int			main(int			argc,
			     char**			argv)
{
  pig::PIG::Main(argc, argv);

  expose();

  return (0);
}
