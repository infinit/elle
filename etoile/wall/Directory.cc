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
// updated       julien quintard   [fri aug 14 18:55:58 2009]
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
      // XXX appeler components::Directory::Load()
      // XXX en gros il ne se passe rien ici, juste un wrapper qui fournit
      // XXX un contexte au lieu de l'objet directement.

      leave();
    }

  }
}
