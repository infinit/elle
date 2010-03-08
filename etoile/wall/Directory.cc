//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/wall/Directory.cc
//
// created       julien quintard   [fri aug 14 16:34:43 2009]
// updated       julien quintard   [wed mar  3 16:40:45 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Directory.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Directory::Load(const Path&		path,
					Identifier&		identifier)
    {
      enter();

      // XXX appeler components::Directory::Load()
      // XXX en gros il ne se passe rien ici, juste un wrapper qui fournit
      // XXX un contexte au lieu de l'objet directement.

      leave();
    }

  }
}
