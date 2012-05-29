#ifndef ETOILE_GEAR_ACTOR_HH
# define ETOILE_GEAR_ACTOR_HH

# include <map>

# include <elle/types.hh>

# include <etoile/gear/Identifier.hh>
# include <etoile/gear/Operation.hh>

namespace etoile
{
  namespace gear
  {

    class Scope;

    ///
    /// an actor represents an application operating on an object for
    /// instance.
    ///
    /// actors have been introduced in order to allow multiple of them
    /// to operate on the same scope i.e a directory, a file etc.
    ///
    /// every actor has a state which indicates whether the actor has
    /// updated or not the scope. by maintaining such a state, the system
    /// is able to reject some operations such as modifying an object before
    /// discarding these updates. indeed, since every modification is
    /// automatically applied on the object, the system cannot come back
    /// by undo-ing them. therefore, the Operate() method is called before
    /// performing any action so that the actor's state can be checked.
    ///
    /// note that this class also maintains a static data structure holding
    /// all the actors, indexed by a unique identifier which is returned
    /// and used by the application for referencing the actor.
    ///
    class Actor:
      public elle::radix::Object
    {
    public:
      //
      // enumerations
      //
      enum State
        {
          StateClean,
          StateUpdated
        };

      //
      // types
      //
      typedef std::map<const Identifier,
                       Actor*>                          Container;
      typedef Container::iterator                       Iterator;
      typedef Container::const_iterator                 Scoutor;

      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Clean();

      static elle::Status       Add(const Identifier&,
                                    Actor*);
      static elle::Status       Select(const Identifier&,
                                       Actor*&);
      static elle::Status       Remove(const Identifier&);

      static elle::Status       Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      static Container          Actors;

      //
      // constructors & destructors
      //
      Actor(Scope*);
      ~Actor();

      //
      // methods
      //
      elle::Status      Operate(const Operation);

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Identifier        identifier;
      Scope*            scope;
      State             state;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Scope.hh>

#endif
