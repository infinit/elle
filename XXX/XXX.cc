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
#include <etoile/Etoile.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <hole/Hole.hh>

#include <XXX/Porcupine.hh>
#include <XXX/Nest.hh>

int main(int argc, char** argv)
{
  // allocate a new parser.
  Infinit::Parser = new elle::Parser(argc, argv);

  elle::Elle::Initialize();
  nucleus::Nucleus::Initialize();
  lune::Lune::Initialize();
  Infinit::Initialize();

  Infinit::Parser->Parse();

  // XXX
  Infinit::Network = "kapoue";
  hole::Hole::Initialize();

  expose();

  // XXX
  nucleus::Porcupine<>::Initialize();

  nucleus::Porcupine<nucleus::Catalog>* p =
    new nucleus::Porcupine<nucleus::Catalog>;

  nucleus::Nest::Setup(
    elle::Callback<>::Infer(&etoile::Nest::Load),
    elle::Callback<>::Infer(&etoile::Nest::Unload));

  const int n = 50000;

  std::vector<elle::String> v(n);

  for (int i = 0; i < n; i++)
    {
      elle::String s;

      elle::Random::Generate(s, 16);

      v[i] = s;
    }

  for (int i = 0; i < n; i++)
    {
      printf("-------------> %s\n", v[i].c_str());

      if (p->Add(v[i], new nucleus::Catalog) == elle::StatusError)
        fail("XXX");
    }

  if (p->Check() == elle::StatusError)
    fail("XXX");

  //p->Dump();

  for (int i = 0; i < n; i++)
    {
      nucleus::Catalog* c;

      printf("-------------= %s\n", v[i].c_str());

      if (p->Locate(v[i], c) == elle::StatusError)
        fail("XXX");
    }

  if (p->Check() == elle::StatusError)
    fail("XXX");

  //p->Dump();

  for (int i = 0; i < n; i++)
    {
      printf("-------------< %s\n", v[i].c_str());

      if (p->Remove(v[i]) == elle::StatusError)
        fail("XXX");
    }

  if (p->Check() == elle::StatusError)
    fail("XXX");

  p->Dump();

  delete p;

  expose();

  delete Infinit::Parser;

  hole::Hole::Clean();
  Infinit::Clean();
  lune::Lune::Clean();
  nucleus::Nucleus::Clean();
  elle::Elle::Clean();

  return 0;
}
