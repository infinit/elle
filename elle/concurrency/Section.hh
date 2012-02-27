//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri oct 28 12:27:21 2011]
//

#ifndef ELLE_CONCURRENCY_SECTION_HH
#define ELLE_CONCURRENCY_SECTION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Entity.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Closure.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this defines the generic Section class enabling specializations.
    ///
    template <typename... T>
    class Section;

    ///
    /// this is the class representing a critical section.
    ///
    /// this type takes two sets of parameters which are both used for
    /// building closures.
    ///
    /// the first set is used to build the closure representing the locking
    /// functionality while the other one representing the unlocking.
    ///
    /// the following illustrates a critical section based on a hurdle i.e
    /// a read/write lock. first two closures are given to the Section
    /// in order for the class to know how to enter and leave the critical
    /// section. then, should an error occured within the section---i.e the
    /// section is not left properly---the system takes care of it
    /// automatically.
    ///
    /// note that Hurdle::S designates the Hurdle-specific Section class,
    /// while Hurdle::L and Hurdle::U provide the closure types for the
    /// lock and unlock methods. finally, Hurdle::C provides a shortcut for
    /// Hurdle-specific callbacks.
    ///
    ///   Hurdle        hurdle;
    ///
    ///   Hurdle::S     section(
    ///     Hurdle::L(Hurdle::C(&Hurdle::Lock, &hurdle)),
    ///     Hurdle::U(Hurdle::C(&Hurdle::Unlock, &hurdle)));
    ///
    ///   section.Enter();
    ///   {
    ///     /* critical section entered */
    ///
    ///     escape("this action will automatically trigger section.Leave()");
    ///   }
    ///   section.Leave();
    ///
    /// note however that concurrency control object generally provide
    /// classes for easing such behaviours. for instance, the Hurdle class
    /// provides a Locker helper class.
    ///
    template <typename... L,
              typename... U>
    class Section< Parameters<L...>,
                   Parameters<U...> >:
      public Entity
    {
    public:
      //
      // enumerations
      //
      enum State
        {
          StateUnlocked,
          StateLocked
        };

      //
      // constructors & destructors
      //
      Section(Closure<
                Void,
                Parameters<L...>,
                Parameters<>
                >,
              Closure<
                Void,
                Parameters<U...>,
                Parameters<>
                >);
      Section(Closure<
                Void,
                Parameters<L...>,
                Parameters<>
                >&,
              Closure<
                Void,
                Parameters<U...>,
                Parameters<>
                >&);
      ~Section();

      //
      // methods
      //
      Void              Enter();
      Void              Leave();

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      State             state;

      Closure<
        Void,
        Parameters<L...>,
        Parameters<>
        >               lock;
      Closure<
        Void,
        Parameters<U...>,
        Parameters<>
        >               unlock;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Section.hxx>

#endif
