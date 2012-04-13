//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// author        julien quintard   [tue may  4 22:00:42 2010]
//

#ifndef INFINIT_HH
#define INFINIT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
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
  static const elle::String             Key;
  static const elle::String             Version;
  static const elle::String             Copyright;

  //
  // globals
  //
  static lune::Authority                Authority;
  static lune::Configuration            Configuration;

  static elle::Parser*                  Parser;

  static elle::String                   User;
  static elle::String                   Network;
  static elle::String                   Mountpoint;

  //
  // methods
  //
  static elle::Status           Initialize();
  static elle::Status           Clean();
};

#endif
