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

///
/// XXX
///
elle::Status            Nest::Load(nucleus::Handle&             handle)
{
  // XXX printf("etoile::nest::Nest::Load()\n");

  // XXX si placement!=null -> recupere de placement container.
  // XXX sinon -> recuperer via address (in which case address!=some/null)

  // XXX set state as loaded

  // XXX
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
