//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [mon feb 16 19:12:33 2009]
//

#ifndef ELLE_ELLE_HH
#define ELLE_ELLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>

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
  using namespace radix;

//
// ---------- classes ---------------------------------------------------------
//

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
    static Status	Initialize();
    static Status	Clean();
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/system/System.hh>
#include <elle/standalone/Standalone.hh>
#include <elle/radix/Radix.hh>
#include <elle/package/Package.hh>
#include <elle/io/IO.hh>
#include <elle/cryptography/Cryptography.hh>
#include <elle/concurrency/Concurrency.hh>
#include <elle/network/Network.hh>
#include <elle/utility/Utility.hh>
#include <elle/idiom/Idom.hh>

#include <elle/Manifest.hh>

//
// ---------- namespaces ------------------------------------------------------
//

namespace elle
{
  using namespace core;
  using namespace system;
  using namespace standalone;
  using namespace radix;
  using namespace package;
  using namespace io;
  using namespace cryptography;
  using namespace concurrency;
  using namespace network;
  using namespace utility;
}

#endif
