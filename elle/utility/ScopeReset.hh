//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri jan 13 19:27:42 2012]
//

#ifndef ELLE_UTILITY_SCOPERESET_HH
# define ELLE_UTILITY_SCOPERESET_HH

namespace elle
{
  namespace utility
  {
    ///
    /// XXX \todo rework that stuff! could not we do without it since
    ///        it is almost never used.
    ///
    /// RAII scope reset
    /// This class is an helper to reset a variable at the end
    /// of a scope (when ScopeReset is destroyed).
    ///
    /// @code
    /// {
    ///   ScopeReset sc_reset(this->IsRunning, 0);
    ///   this->IsRunning = 1;
    ///   ...
    /// } // here this->IsRunning = 0;
    /// @endcode
    ///
    template <typename T>
    class ScopeReset
    {
    public:

      inline ScopeReset(T & var, const T value)
        : var_(var),
          value_(value)
      {
      }

      inline ~ScopeReset()
      {
        var_ = value_;
      }

    private:

      T &     var_;
      const T value_;
    };
  }
}

#endif
