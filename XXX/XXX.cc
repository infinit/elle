//
// ---------- header ----------------------------------------------------------
//
// project       XXX.cc
//
// license       infinit
//
// author           [thu sep 22 08:57:11 2011]
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <hole/Hole.hh>

#include <XXX/Porcupine.hh>

int main(int argc, char** argv)
{
  enter();

  // allocate a new parser.
  Infinit::Parser = new elle::Parser(argc, argv);

  elle::Elle::Initialize();
  nucleus::Nucleus::Initialize();
  lune::Lune::Initialize();
  Infinit::Initialize();
  hole::Hole::Options();

  Infinit::Parser->Parse();

  hole::Hole::Initialize();

  expose();

  // XXX
  nucleus::Porcupine<>::Initialize();

  nucleus::Porcupine<nucleus::Catalog>* p =
    new nucleus::Porcupine<nucleus::Catalog>;

  const int n = 2000;

  for (int i = 0; i < n; i++)
    {
      char name[1024];

      memset(name, 0x0, sizeof (name));

      sprintf(name, "%u", i);

      printf("-------------> %s\n", name);

      if (p->Add(name, new nucleus::Catalog) == elle::StatusError)
	fail("XXX");
    }

  //p->Dump();

  for (int i = 0; i < n; i++)
    {
      nucleus::Catalog*	c;
      char name[1024];

      memset(name, 0x0, sizeof (name));

      sprintf(name, "%u", i);

      printf("-------------= %s\n", name);

      if (p->Locate(name, c) == elle::StatusError)
	fail("XXX");
    }

  //p->Dump();

  for (int i = 0; i < n; i++)
    {
      char name[1024];

      memset(name, 0x0, sizeof (name));

      sprintf(name, "%u", i);

      printf("-------------< %s\n", name);

      if (p->Remove(name) == elle::StatusError)
	fail("XXX");
    }

  p->Dump();

  delete p;

  expose();

  delete Infinit::Parser;
 
  hole::Hole::Clean();
  Infinit::Clean();
  lune::Lune::Clean();
  nucleus::Nucleus::Clean();
  elle::Elle::Clean();

  release();

  return 0;
}
