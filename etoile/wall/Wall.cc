//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/wall/Wall.cc
//
// created       julien quintard   [fri aug 14 12:57:57 2009]
// updated       julien quintard   [sun feb  7 14:57:17 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Wall.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// XXX
    ///
    Wall::Container		Wall::Interface;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the wall by setting up the interface callbacks.
    ///
    Status		Wall::Initialize()
    {
      //Wall::Register<Identifier, Information>("Object::Meta", &Object::Meta);

      // XXX ensuite on fait Bridge::Listen avec comme callback une methode
      // statique de Wall qui, une fois recue un message, extrait son nom
      // (String) et lance une fonctionoid qui extrait les bons types et
      // declenche le callback du fonctionoid.

      // XXX fournir aussi Wall::Request() qui est template et juste concatene
      // les types comme ca pas besoin d'avoir un message par arguments
      // d'inputs sinon ca va etre la merde.

      leave();
    }

    ///
    /// this method cleans the callbacks.
    ///
    Status		Wall::Clean()
    {
      // XXX remove callbacks

      leave();
    }

  }
}
