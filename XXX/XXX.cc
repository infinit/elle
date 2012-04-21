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
#include <elle/types.hh>
#include <etoile/Etoile.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <hole/Hole.hh>

#include <XXX/Porcupine.hh>
#include <XXX/Pins.hh>

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
  nucleus::Porcupine<>::Initialize(
    elle::concurrency::Callback<>::Infer(&etoile::Nest::Attach),
    elle::concurrency::Callback<>::Infer(&etoile::Nest::Detach),
    elle::concurrency::Callback<>::Infer(&etoile::Nest::Load),
    elle::concurrency::Callback<>::Infer(&etoile::Nest::Unload));

  nucleus::Porcupine<nucleus::Catalog>* p =
    new nucleus::Porcupine<nucleus::Catalog>;

  const int n = 10;

  std::vector<elle::String> v(n);

  for (int i = 0; i < n; i++)
    {
      elle::String s;

      cryptography::Random::Generate(s, 16);

      v[i] = s;
    }

  /* XXX
  for (int i = 0; i < n; i++)
    {
      char buffer[128];

      sprintf(buffer, "%d", i);

      elle::cryptography::Digest digest;

      elle::cryptography::OneWay::Hash(elle::Region((elle::Byte*)buffer, strlen(buffer)),
                         digest);

      elle::String s;

      elle::Hexadecimal::Encode(digest.region, s);

      v[i] = s.substr(0, 16);
    }
  */

  for (int i = 0; i < n; i++)
    {
      printf("[%u] -------------> %s\n", i, v[i].c_str());

      Handle value;
      nucleus::Contents< nucleus::Catalog >*    contents =
        new nucleus::Contents<nucleus::Catalog>;

      contents->Create();

      if (nucleus::Porcupine<>::Attach.Call(
            contents,
            value) == elle::Status::Error)
        fail("unable to attach the value");

      if (p->Add(v[i], value) == elle::Status::Error)
        fail("XXX");
    }

  if (p->Check(nucleus::PinParent |
               nucleus::PinNeighbours |
               nucleus::PinKey) == elle::Status::Error)
    fail("XXX");

  //p->Dump();

  for (int i = 0; i < n; i++)
    {
      nucleus::Handle   h;

      printf("[%u] -------------= %s\n", i, v[i].c_str());

      if (p->Locate(v[i], h) == elle::Status::Error)
        fail("XXX");
    }

  if (p->Check(nucleus::PinParent |
               nucleus::PinNeighbours |
               nucleus::PinKey) == elle::Status::Error)
    fail("XXX");

  elle::cryptography::SecretKey sk;

  if (sk.Generate(128) == elle::Status::Error)
    fail("XXX");

  //p->Traverse();

  if (p->Seal(sk) == elle::Status::Error)
    fail("unable to seal the porcupine");

  //p->Traverse();

  if (p->Check(nucleus::PinAll) == elle::Status::Error)
    fail("XXX");

  p->Traverse();

  for (int i = 0; i < n; i++)
    {
      nucleus::Handle   h;

      printf("[%u] -------------< %s\n", i, v[i].c_str());

      if (p->Locate(v[i], h) == elle::Status::Error)
        fail("XXX");

      if (p->Remove(v[i]) == elle::Status::Error)
        fail("XXX");

      if (nucleus::Porcupine<>::Detach.Call(h) == elle::Status::Error)
        fail("unable to detach the value");
    }

  p->Dump();

  etoile::Nest::Show();

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
