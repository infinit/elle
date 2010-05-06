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
// updated       julien quintard   [wed may  5 21:02:53 2010]
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
    // static methods
    //
    static elle::Status	Initialize();
    static elle::Status	Clean();

    //
    // static attributes
    //
    static elle::String		Home;

    static elle::String		Users;
    static elle::String		Passports;

    static elle::String		Universes;
    static elle::String		Reserve;
    static elle::String		Hole;

    static elle::String		Authority;
    static elle::String		Phrase;
    static elle::String		Access;
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Authority.hh>
#include <lune/Memento.hh>
#include <lune/Identity.hh>
#include <lune/Phrase.hh>

#endif
