//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/configuration/Configuration.hh
//
// created       julien quintard   [mon aug 10 12:13:56 2009]
// updated       julien quintard   [tue mar 16 21:40:18 2010]
//

#ifndef ETOILE_CONFIGURATION_CONFIGURATION_HH
#define ETOILE_CONFIGURATION_CONFIGURATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  ///
  /// this name space contains everything related to etoile's configuration.
  ///
  namespace configuration
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class manages the etoile's configuration.
    ///
    class Configuration
    {
    public:
      //
      // static attributes
      //
      struct				Path
      {
	static Natural32		Capacity;
      };

      struct				Cache
      {
	static Natural64		Capacity;
      };

      struct				Reserve
      {
	static String			Path;
	static Natural64		Capacity;
      };

      struct				Wall
      {
	static String			Line;
      };

      //
      // static methods
      //
      static Status	Initialize(const String&);
      static Status	Clean();
    };

    // XXX ajouter depot/Repository/Delays enfin tous les delays (path, depot)
    // et revoir le terme "delays"

    // XXX dans depot/repo et path/cache, le Evict devrait etre fait a la fin
    // de sorte qu'on depasse la capacite mais a la fin en reequilibre. on
    // pourrait nommer la fonction Balance d'ailleurs.

  }
}

#endif
