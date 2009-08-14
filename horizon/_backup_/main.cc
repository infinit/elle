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
// updated       julien quintard   [fri jul 31 22:19:15 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <pig/system/fuse/System.hh>

int			main(int			argc,
			     char**			argv)
{
  pig::system::System::Main(argc, argv);

  expose();

  return (0);
}
