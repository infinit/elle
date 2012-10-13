#include <etoile/gear/Guard.hh>
#include <etoile/gear/Actor.hh>
#include <etoile/gear/Scope.hh>

#include <elle/standalone/Report.hh>
#include <elle/idiom/Open.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.etoile.gear.Guard");

namespace etoile
{
  namespace gear
  {

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
              if (Scope::Annihilate(this->_scope) == elle::Status::Error)
                {
                  ELLE_WARN("unable to annihilate the scope");
                }
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

      return (elle::Status::Ok);
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

  }
}
