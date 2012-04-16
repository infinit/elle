#ifndef ELLE_CONCURRENCY_ENTRANCE_HH
# define ELLE_CONCURRENCY_ENTRANCE_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>
# include <elle/radix/Parameters.hh>

# include <elle/concurrency/Closure.hh>
# include <elle/concurrency/Timer.hh>

namespace elle
{

  namespace concurrency
  {

    ///
    /// this generic class enables specializations.
    ///
    template <typename... T>
    class Entrance;

    ///
    /// this class represents a closure which is triggered after some
    /// time, by default, at the next iterator of the event loop.
    ///
    /// in other words, this class transforms a closure into a sophisticated
    /// event since it carries information i.e the potential arguments.
    ///
    template <typename R,
              typename... T>
    class Entrance< R,
                    Parameters<T...> >:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Entrance(const Closure< R, Parameters<T...> >&);

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // callbacks
      //
      Status            Timeout();

      //
      // attributes
      //
      Closure< R, Parameters<T...> >    closure;
      Timer                             timer;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Entrance.hxx>

#endif
