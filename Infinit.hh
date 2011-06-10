//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/Infinit.hh
//
// created       julien quintard   [tue may  4 22:00:43 2010]
// updated       julien quintard   [thu jun  2 15:31:58 2011]
//

#ifndef INFINIT_HH
#define INFINIT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <lune/Lune.hh>

//
// ---------- classes ---------------------------------------------------------
//

class Infinit
{
public:
  //
  // constants
  //
  static const elle::String		Authority;
  static const elle::String		Version;

  //
  // globals
  //
  static lune::Descriptor		Descriptor;
  static lune::Identity			Identity;

  //
  // methods
  //
  static elle::Status		Initialize(const elle::String&,
					   const elle::String&);
  static elle::Status		Clean();
};

#endif
