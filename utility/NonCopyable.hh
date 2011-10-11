//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        Alexandre Bique   [tue oct 11 13:44:22 2011]
//

namespace elle
{
  namespace utility
  {
    ///
    /// This is an helper to make a class non-copyable
    ///
    class NonCopyable
    {
    private:
      /// disable copy constructor
      NonCopyable(const NonCopyable &);
    };
  }
}
