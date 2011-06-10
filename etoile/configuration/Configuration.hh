//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/configuration/Configuration.hh
//
// created       julien quintard   [mon aug 10 12:13:56 2009]
// updated       julien quintard   [wed jun  1 11:31:42 2011]
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
	static const elle::Natural32	Capacity;
      };

      struct				Cache
      {
	static const elle::Natural64	Capacity;
      };

      struct				Reserve
      {
	static const elle::Natural64	Capacity;
      };

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();
    };

    // XXX ajouter depot/Repository/Delays enfin tous les delays (path, depot)
    // et revoir le terme "delays"

    // XXX dans depot/repo et path/cache, le Evict devrait etre fait a la fin
    // de sorte qu'on depasse la capacite mais a la fin en reequilibre. on
    // pourrait nommer la fonction Balance d'ailleurs.

  }
}

#endif
