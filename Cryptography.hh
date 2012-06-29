#ifndef ELLE_CRYPTOGRAPHY_CRYPTOGRAPHY_HH
# define ELLE_CRYPTOGRAPHY_CRYPTOGRAPHY_HH

# include <elle/types.hh>

namespace elle
{
  ///
  /// this namespace contains everything necessary for performing
  /// cryptographic operations.
  ///
  namespace cryptography
  {

    ///
    /// this class represents the cryptography package and provides a
    /// Initialise() method for initialising the package's components.
    ///
    /// this class also provides a general interface for cryptographic
    /// operations.
    ///
    /// note that both Initialize() and Clean() should be called in an
    /// upper sccope to any other cryptographic operations. indeed, otherwise,
    /// the Clean() would release every resource and would prevents live
    /// cryptograhic object from being properly deleted since destructors
    /// usually call _free() functions and those will no longer work after
    /// Clean() has been called.
    ///
    /// the same is true for static objects. since those objects will be
    /// deleted when the application stops and since Clean() will probably
    /// have been called, hence before, these static objects will never be
    /// properly deleted. one should therefore avoid using such static
    /// objects or use static pointers with initialization and cleaning
    /// methods.
    ///
    /// note that Initialize() and Clean() should be called from a single
    /// thread as, obviously, these method are *not* thread-safe.
    ///
    class Cryptography
    {
    public:
      //
      // static methods
      //
      static Status             Initialize();
      static Status             Clean();
    };

  }
}

#endif
