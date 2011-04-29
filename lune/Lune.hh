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
// updated       julien quintard   [tue apr 26 15:10:43 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#ifndef LUNE_LUNE_HH
#define LUNE_LUNE_HH

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
    static elle::Status		Initialize();
    static elle::Status		Clean();

    //
    // static attributes
    //
    static elle::Pattern	Authority;
    static elle::Pattern	User;
    static elle::Pattern	Identity;
    static elle::Pattern	Dictionary;
    static elle::Pattern	Phrase;
    static elle::Pattern	Network;
    static elle::Pattern	Descriptor;
    static elle::Pattern	Reserve;
    static elle::Pattern	Shelter;
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Authority.hh>
#include <lune/Descriptor.hh>
#include <lune/Dictionary.hh>
#include <lune/Identity.hh>
#include <lune/Phrase.hh>

#endif
