#ifndef ELLE_CONCURRENCY_ENVIRONMENT_HH
# define ELLE_CONCURRENCY_ENVIRONMENT_HH


# include <elle/types.hh>

# include <elle/radix/Meta.hh>
# include <elle/radix/Entity.hh>

# include <elle/idiom/Close.hh>
#  include <vector>
# include <elle/idiom/Open.hh>

namespace elle
{

  using namespace radix;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is used to store and restore global information
    /// whenever fibers are switched.
    ///
    /// note that the environment stores pointers and that template
    /// methods are used to avoid cast though, in this inside, casts
    /// are made.
    ///
    class Environment:
      public Entity
    {
    public:
      //
      // types
      //
      typedef std::pair<String, Meta*>          Value;
      typedef std::vector<Value>                Container;
      typedef Container::iterator               Iterator;
      typedef Container::const_iterator         Scoutor;

      //
      // methods
      //
      template <typename T>
      Status            Store(const String&,
                              T*);
      template <typename T>
      Status            Load(const String&,
                             T*&);

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Container         container;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Environment.hxx>

#endif
