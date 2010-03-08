//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/configuration/Configuration.cc
//
// created       julien quintard   [mon aug 10 16:46:56 2009]
// updated       julien quintard   [fri mar  5 10:24:04 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/configuration/Configuration.hh>

namespace etoile
{
  namespace configuration
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// path-specific configuration attributes.
    ///
    String		Configuration::Path::Separator("/");
    Natural32		Configuration::Path::Capacity = 4096;

    ///
    /// cache-specific configuration attributes.
    ///
    Natural64		Configuration::Cache::Capacity = 2097152;

    ///
    /// reserve-specific configuration attributes.
    ///
    String		Configuration::Reserve::Path;
    Natural64		Configuration::Reserve::Capacity = 1073741824;

    ///
    /// wall-specific configuration attributes
    ///
    String		Configuration::Wall::Line("etoile");

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the configuration, building attributes
    /// that depend on other for instance.
    ///
    Status		Configuration::Initialize(const String&	path)
    {
      enter();

      // build the reserve path.
      Configuration::Reserve::Path =
	path + Configuration::Path::Separator + String("reserve");

      leave();
    }

    ///
    /// this method cleans the configuration resources.
    ///
    Status		Configuration::Clean()
    {
      enter();

      leave();
    }

  }
}
