#ifndef ELLE_ELLE_HH
# define ELLE_ELLE_HH

#include <elle/types.hh>

///
/// the elle namespace is composed of multiple modules, often regarded
/// as layers.
///
/// the core module comes first with its basic type definitions. then
/// the standalone module provides fundamental functionalities, followed
/// by the radix module which serves as a base to many classes. then the
/// package module provides the serialization functionality. the io module,
/// coupled with the package functionality enables classes to display,
/// store etc. in a very easy way. more modules are provided which are not
/// as fundamental as the one cited above.
///
namespace elle
{

  ///
  /// this class represents the main class which contains methods
  /// for controlling the elle library.
  ///
  /// these methods are obviously not thread safe and should therefore
  /// be called only once and probably from the main thread.
  ///
  class Elle
  {
  public:
    //
    // static methods
    //
    static Status       Initialize();
    static Status       Clean();
  };

} // !namespace elle

#endif
