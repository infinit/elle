//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [sun mar 11 19:19:01 2012]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/nest/Nest.hh>

using namespace etoile::nest;

//
// ---------- definitions -----------------------------------------------------
//

///
/// XXX
///
Nest::P::Container      Nest::Placements;

///
/// XXX
///
Nest::A::Container      Nest::Addresses;

//
// ---------- static methods --------------------------------------------------
//

///
/// XXX
///
elle::Status            Nest::Register(nucleus::Handle&         handle)
{
  // XXX printf("etoile::nest::Nest::Register()\n");

  // XXX assert: placement==null

  // XXX ajoute dans placement container

  return elle::StatusOk;
}

/* XXX
   XXX
   - on load un block.
   - on veut loader son frere gauche, on load a partir de l addresse.
   - le nest regarde si il a un mapping entre cette adresse et un block
   deja loade mais qui a ete modifie. si cest le cas, le mapping nous donne
   le placement du block et on le recupere.
   - sinon on recupere a partir de hole.

   - en soit on a 3 cas:
   - si le handle a un pointer, on l utilise.
   - si le handle a un placement, on recupere le block du nest
   - sinon, on prend l address, et on load a partir du nest (mais le
   mapping pourrait nous retourner un block deja loade)

   - a noter que peut etre qu il serait utile de garder le handle dans chaque
   nodule pour eviter de recalculer.
*/

// XXX
/* XXX
   if (this->handle.status == Handle::StatusLoaded)
   ;
*/

///
/// XXX
///
elle::Status            Nest::Load(nucleus::Handle&             handle)
{
  // XXX printf("etoile::nest::Nest::Load()\n");

  // XXX si deja loade, rien a faire (c'est possible)
  // XXX si placement=null & address=null -> error (auquel cas il faudrait rajouter des if (address != null) avant de loader)

  // XXX si placement!=null -> recupere de placement container.
  // XXX sinon -> recuperer via address (in which case address!=some/null)

  // XXX set state as loaded

  // XXX
  handle._block = handle._XXX;
  handle._XXX = nullptr;

  return elle::StatusOk;
}

///
/// XXX
///
elle::Status            Nest::Unload(nucleus::Handle&             handle)
{
  // XXX printf("etoile::nest::Nest::Unload()\n");

  // XXX assert placement!=null

  // XXX set state as unloaded

  // XXX
  handle._XXX = handle._block;
  handle._block = nullptr;

  return elle::StatusOk;
}
