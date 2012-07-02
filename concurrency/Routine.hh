#ifndef ELLE_CONCURRENCY_ROUTINE_HH
# define ELLE_CONCURRENCY_ROUTINE_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/idiom/Open.hh>

namespace elle
{

  using namespace radix;

  namespace concurrency
  {

    ///
    /// this class represents a routine, being a function, a method
    /// or static method.
    ///
    class Routine:
      public radix::Object
    {
    public:
      //
      // enumerations
      //
      enum Scheme
        {
          SchemeUnknown,

          SchemeFunction,
          SchemeMethod
        };

      //
      // constructors & destructors
      //
      Routine();
      Routine(const Scheme);

      //
      // interfaces
      //

      // object
      declare(Routine);

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Scheme            scheme;
    };

  }
}

#endif
