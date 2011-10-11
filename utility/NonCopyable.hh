//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        Alexandre Bique   [tue oct 11 13:44:22 2011]
//

#ifndef ELLE_UTILITY_NONCOPYABLE_HH
# define ELLE_UTILITY_NONCOPYABLE_HH

namespace elle
{
  namespace utility
  {
    ///
    /// This is an helper to make a class non-copyable
    ///
    class NonCopyable
    {
    public:
      inline NonCopyable() {}

    private:
      /// disable copy constructor
      NonCopyable(const NonCopyable &);
    };
  }
}

#endif
