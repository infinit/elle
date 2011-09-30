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

class C:
  public elle::Object
{
public:
  typedef elle::Natural32	K;

  C(const elle::String s):
    name(s)
  {
  }

  elle::Status	Dump(const elle::Natural32	margin) const
  {
    elle::String			alignment(margin, ' ');

    enter();

    std::cout << alignment << "[C] " << this->name << std::endl;

    leave();
  }

  elle::Status	Serialize(elle::Archive&	archive) const
  {
    enter();

    archive.Serialize(this->name);

    leave();
  }

  elle::Status	Extract(elle::Archive&		archive)
  {
    enter();

    archive.Extract(this->name);

    leave();
  }

  elle::String	name;
};

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

  nucleus::Porcupine<C> p(elle::Callback<
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

  const int n = 5;

  for (int i = 0; i < n; i++)
    {
      char name[1024];

      memset(name, 0x0, sizeof (name));

      sprintf(name, "entry-%u", i);

      // XXX C* c = new C(name);

      p.Add(i, NULL);
    }

  p.Dump();

  for (int i = 0; i < n; i++)
    {
      C*	c;

      if (p.Locate(i, c) == elle::StatusError)
	exit(1);
    }

  for (int i = 0; i < n; i++)
    {
      printf("-------------------- %u\n", i);

      p.Remove(i);
    }

  p.Dump();

  expose();

  delete Infinit::Parser;
 
  hole::Hole::Clean();
  Infinit::Clean();
  lune::Lune::Clean();
  nucleus::Nucleus::Clean();
  elle::Elle::Clean();

  return 0;
}
