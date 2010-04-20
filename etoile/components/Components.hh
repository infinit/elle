//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Components.hh
//
// created       julien quintard   [fri aug 14 18:56:29 2009]
// updated       julien quintard   [tue apr 20 07:54:29 2010]
//

#ifndef ETOILE_COMPONENTS_COMPONENTS_HH
#define ETOILE_COMPONENTS_COMPONENTS_HH

namespace etoile
{
  ///
  /// this namespace contains classes that provide functionalities to
  /// make the gap between the high level interface, named `wall' and the
  /// low-level components present in kernel/.
  ///
  /// the components classes therefore manipulate low-level objects and
  /// return to the wall high level representations such as State, Window
  /// etc.
  ///
  namespace components
  {

    // nothing
  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Object.hh>
#include <etoile/components/Directory.hh>
#include <etoile/components/Link.hh>
#include <etoile/components/Contents.hh>
#include <etoile/components/Author.hh>
#include <etoile/components/Access.hh>
#include <etoile/components/Rights.hh>

#endif
