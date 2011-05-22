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
// updated       julien quintard   [fri may 20 22:55:34 2011]
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

    static elle::Pattern	Users;
    static elle::Pattern	Networks;

    struct			User
    {
      static elle::Pattern	Root;
      static elle::Pattern	Identity;
      static elle::Pattern	Dictionary;
      static elle::Pattern	Phrase;
    };

    struct			Network
    {
      static elle::Pattern	Root;
      static elle::Pattern	Descriptor;

      struct			Reserve
      {
	static elle::Pattern	Root;

	struct			Block
	{
	  static elle::Pattern	Immutable;
	  static elle::Pattern	Mutable;
	};
      };

      struct			Shelter
      {
	static elle::Pattern	Root;

	struct			Block
	{
	  static elle::Pattern	Immutable;
	  static elle::Pattern	Mutable;
	};
      };
    };
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
