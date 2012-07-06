#ifndef NUCLEUS_NUCLEUS_HH
# define NUCLEUS_NUCLEUS_HH

# include <elle/types.hh>
# include <elle/utility/Factory.hh>

///
/// this namespace contains the whole logic related to the Infinit
/// file system.
///
namespace nucleus
{

  ///
  /// this class controls the nucleus library.
  ///
  class Nucleus
  {
  public:
    //
    // static methods
    //
    static elle::Status         Initialize();
    static elle::Status         Clean();

    //
    // static attributes
    //
    static elle::utility::Factory        Factory;
  };

}

#endif
