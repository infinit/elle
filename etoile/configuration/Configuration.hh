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
// updated       julien quintard   [fri jan 29 16:25:48 2010]
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
	static String			Separator;
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

      //
      // static methods
      //
      Status		Initialize(const String&);
      Status		Clean();
    };

  }
}

#endif
