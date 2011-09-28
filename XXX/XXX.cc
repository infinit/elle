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
  typedef int	K;

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

  p.Create();

  for (int i = 0; i < 50000; i++)
    {
      char name[1024];

      memset(name, 0x0, sizeof (name));

      sprintf(name, "entry-%u", i);

      C* c = new C(name);

      p.Add(i, c);
    }

  p.Dump();

  expose();

  return 0;
}
