//
// ---------- header ----------------------------------------------------------
//
// project       XXX.cc
//
// license       infinit
//
// author           [thu sep 22 08:57:11 2011]
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

int main()
{
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

  nucleus::Catalog* x = new nucleus::Catalog;
  nucleus::ContentHashBlock	chb;
  nucleus::Address		a;

  chb.Bind(a);

  for (int i = 0; i < 5; i++)
    {
      char name[1024];

      sprintf(name, "e%u", i);

      nucleus::Entry* e = new nucleus::Entry(name, a);

      x->Add(e);
    }

  p.Create();
  p.Add("e100", x);
  p.Dump();

  printf("footprint: %u\n", x->Footprint());

  expose();

  return 0;
}
