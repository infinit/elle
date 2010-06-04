//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Lune.hh
//
// created       julien quintard   [fri apr 30 12:55:13 2010]
// updated       julien quintard   [fri may 28 18:09:51 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#ifndef LUNE_HH
#define LUNE_HH

///
/// this namespace contains everything necessary to manipulate satellite
/// nodes, especially their local information such as local keys,
/// locally stored data and so on.
///
namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class provides functionalities for manipulating the lune
  /// library.
  ///
  class Lune
  {
  public:
    //
    // enumerations
    //
    enum Mode
      {
	ModeSystem,
	ModeUser
      };

    //
    // static methods
    //
    static elle::Status		Initialize();
    static elle::Status		Clean();

    static elle::Status		Select(const elle::String&);

    //
    // static attributes
    //
    static Mode			Environment;

    struct			System
    {
      static elle::String	Root;

      static elle::String	Identities;
      static elle::String	Universes;
      static elle::String	Passports;

      static elle::String	Authority;
    };

    struct			User
    {
      static elle::String	Name;

      static elle::String	Home;

      static elle::String	Universes;
      static elle::String	Passports;
      static elle::String	Reserve;
      static elle::String	Hole;
    };
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Associat.hh>
#include <lune/Authority.hh>
#include <lune/Chain.hh>
#include <lune/Memento.hh>
#include <lune/Identity.hh>
#include <lune/Passport.hh>
#include <lune/Phrase.hh>

#endif
