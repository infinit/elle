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

#include <Infinit.hh>

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
elle::Status            Nest::Attach(nucleus::Block*            block,
                                     nucleus::Handle&           handle)
{
  std::pair<Nest::P::Iterator, elle::Boolean>   result;
  nucleus::Placement                            placement;

  // debug.
  if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] nest::Nest::Attachh()\n");

  // make sure placement is null.
  assert(handle.placement == nucleus::Placement::Null);

  // generate a placement.
  if (nucleus::Placement::Generate(placement) == elle::StatusError)
    escape("unable to generate a unique placement");

  // create a new selectionoid.
  auto                                          pod =
    std::unique_ptr<Pod>(new Pod(Pod::NatureVolatile, block));

  // insert the selectionoid in the container.
  result =
    Nest::Placements.insert(
      std::pair<const nucleus::Placement, Pod*>(handle._placement, pod.get()));

  // check if the insertion was successful.
  if (result.second == false)
    escape("unable to insert the pod in the container");

  // release track.
  pod.release();

  // return a handle.
  handle = nucleus::Handle(placement);

  return elle::StatusOk;
}

///
/// XXX
///
elle::Status            Nest::Detach(nucleus::Handle&           handle)
{
  // debug.
  if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] nest::Nest::Detach()\n");

  // make sure placement is non-null.
  assert(handle.placement != nucleus::Placement::Null);

  // XXX ajoute dans placement container

  return elle::StatusOk;
}

///
/// XXX
///
elle::Status            Nest::Load(nucleus::Handle&             handle)
{
  // debug.
  if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] nest::Nest::Load()\n");

  // make sure the given handle is valid.
  assert((handle.placement != nucleus::Placement::Null) ||
         (handle.address != nucleus::Address::Null));
  assert(!((handle.placement != nucleus::Placement::Null) &&
           (handle.address != nucleus::Address::Null)));

  // XXX si deja loade, rien a faire (c'est possible)
  // XXX si placement=null & address=null -> error (auquel cas il faudrait rajouter des if (address != null) avant de loader)

  // XXX si placement!=null -> recupere de placement container.
  // XXX sinon -> recuperer via address (in which case address!=some/null)

  // XXX set state as loaded

  return elle::StatusOk;
}

///
/// XXX
///
elle::Status            Nest::Unload(nucleus::Handle&             handle)
{
  // debug.
  if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] nest::Nest::Unload()\n");

  // XXX assert placement!=null

  // XXX set state as unloaded

  return elle::StatusOk;
}
