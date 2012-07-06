#ifndef ETOILE_ETOILE_HH
# define ETOILE_ETOILE_HH

#include <elle/types.hh>

///
/// the principal Etoile namespace which provides functionalities for
/// manipulating the file system objects.
///
namespace etoile
{

  ///
  /// this class controls Etoile.
  ///
  class Etoile
  {
  public:
    //
    // static methods
    //
    static elle::Status Initialize();
    static elle::Status Clean();
  };

}

#endif
