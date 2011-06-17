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
// updated       julien quintard   [fri jun 17 17:06:08 2011]
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
  static const elle::String		Key;
  static const elle::String		Version;

  //
  // globals
  //
  static lune::Authority		Authority;

  //
  // methods
  //
  static elle::Status		Initialize();
  static elle::Status		Clean();
};

#endif
