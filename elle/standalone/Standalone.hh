#ifndef ELLE_STANDALONE_HH
# define ELLE_STANDALONE_HH

# include <elle/types.hh>

namespace elle
{
  ///
  /// this namespace contains classes that must depend on _almost_ nothing
  /// as the whole Elle library relies on them.
  ///
  namespace standalone
  {

    class Standalone
    {
    public:
      static Status     Initialize();
      static Status     Clean();
    };

  }
}

#endif
