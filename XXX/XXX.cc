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

  nucleus::Porcupine<nucleus::Catalog> p(elle::Callback<
					   elle::Status,
					   elle::Parameters<
					     const nucleus::Address&,
					     nucleus::Block&
					     >
					   >::Null,
					 elle::Callback<
					   elle::Status,
					   elle::Parameters<
					     const nucleus::Address&,
					     const nucleus::Block&
					     >
					   >::Null);

  nucleus::ContentHashBlock	chb;
  nucleus::Address		a;
  int i = 0;

  chb.Bind(a);

  p.Create();

  for (int j = 0; j < 5; j++)
    {
      nucleus::Catalog* x = new nucleus::Catalog;
      char name[1024];

      do
	{
	  memset(name, 0x0, sizeof (name));

	  sprintf(name, "e%u", i);

	  nucleus::Entry* e = new nucleus::Entry(name, a);

	  x->Add(e);
	} while (0);

      i++;

      p.Add(name, x);
    }

  //p.Dump();

  expose();

  return 0;
}
