#ifndef HORIZON_MACOSX_JANITOR_HH
# define HORIZON_MACOSX_JANITOR_HH

#include <elle/types.hh>

namespace horizon
{
  namespace macosx
  {

    ///
    /// the janitor is responsible for discarding the identifiers it is
    /// given.
    ///
    /// the janitor is called whenever an unexpected error occurs in the
    /// crux.
    ///
    class Janitor
    {
    public:
      //
      // static methods
      //
      static elle::Void         Clear();

      template <typename T>
      static elle::Void         Clear(T&);
      template <typename T,
                typename... TT>
      static elle::Void         Clear(T&,
                                      TT&...);
    };

  }
}

#include <horizon/macosx/Janitor.hxx>

#endif
