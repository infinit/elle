//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/configuration/Configuration.cc
//
// created       julien quintard   [mon aug 10 16:46:56 2009]
// updated       julien quintard   [mon may  3 12:46:02 2010]
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
    const elle::Natural32	Configuration::Path::Capacity = 4096;

    ///
    /// cache-specific configuration attributes.
    ///
    const elle::Natural64	Configuration::Cache::Capacity = 2097152;

    ///
    /// reserve-specific configuration attributes.
    ///
    const elle::Natural64	Configuration::Reserve::Capacity = 1073741824;

    ///
    /// wall-specific configuration attributes
    ///
    const elle::String		Configuration::Wall::Line("etoile");

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the configuration, building attributes
    /// that depend on other for instance.
    ///
    elle::Status	Configuration::Initialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method cleans the configuration resources.
    ///
    elle::Status	Configuration::Clean()
    {
      enter();

      // nothing to do.

      leave();
    }

  }
}
