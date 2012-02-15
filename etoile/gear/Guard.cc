//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [tue feb 14 19:32:41 2012]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Guard.hh>

using namespace etoile::gear;

//
// ---------- constructors & destructors --------------------------------------
//

///
/// default constructor.
///
Guard::Guard(Scope*                                             scope,
             Actor*                                             actor):
  _scope(scope),
  _actor(actor),
  _track(true)
{
}

///
/// another constructor.
///
Guard::Guard(Actor*                                             actor):
  _scope(nullptr),
  _actor(actor),
  _track(true)
{
}

///
/// destructor.
///
Guard::~Guard()
{
  if (this->_track == true)
    {
      if (this->_actor != nullptr)
        delete this->_actor;

      if (this->_scope != nullptr)
        {
          if (Scope::Annihilate(this->_scope) == elle::StatusError)
            log("unable to annihilate the scope");
        }
    }
}

//
// ---------- methods ---------------------------------------------------------
//

///
/// this method releases the scope guarding.
///
elle::Status            Guard::Release()
{
  this->_track = false;

  return (elle::StatusOk);
}

//
// ---------- getters & setters -----------------------------------------------
//

Actor*                  Guard::actor()
{
  return (this->_actor);
}

elle::Void              Guard::actor(Actor*                     actor)
{
  //
  // delete the previous actor, if different.
  //
  if (this->_actor != actor)
    delete this->_actor;

  this->_actor = actor;
}
