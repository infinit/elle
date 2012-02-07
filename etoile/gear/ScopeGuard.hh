//
// ---------- header ----------------------------------------------------------
//
// project       @FIXME@
//
// license       infinit
//
// author        @NAME@   [Mon 06 Feb 2012 03:55:00 PM CET]
//

#ifndef ETOILE_GEAR_SCOPEGUARD_HH
#define ETOILE_GEAR_SCOPEGUARD_HH

//
// ---------- includes --------------------------------------------------------
//

#include "etoile/gear/Scope.hh"
#include "etoile/gear/Actor.hh"

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    struct ScopeGuard
    {
    private:
      Scope*  _scope;
      Actor*  _actor;
      bool    _track;

    public:
      ScopeGuard(Scope* scope, Actor* actor = nullptr) :
        _scope(scope), _actor(actor), _track(true)
      {}
      ScopeGuard(Actor* actor) :
        _scope(nullptr), _actor(actor), _track(true)
      {}
      ~ScopeGuard()
      {
        if (!this->_track)
          return;
        delete this->_actor;
        if (this->_scope)
          Scope::Annihilate(this->_scope);
      }
      void release()
      {
        this->_track = false;
      }

      Actor* actor(Actor* new_actor)
      {
        if (this->_actor != new_actor)
          {
            delete this->_actor;
            this->_actor = new_actor;
          }
        return this->_actor;
      }
      Actor* actor()
      {
        return this->_actor;
      }
    };

  }
}


#endif /* ! SCOPEGUARD_HH */
