#ifndef ELLE_SCOPED_ASSIGNMENT_HH
# define ELLE_SCOPED_ASSIGNMENT_HH

namespace elle
{
  /// Set a value and reset it at the end if the scope.
  ///
  /// \code{.cc}
  ///
  /// void
  /// foo()
  /// {
  ///   int i = 3;
  ///   {
  ///     assert(i == 3);
  ///     elle::ScopedAssignment guard_i(i, 40);
  ///     assert(i == 40);
  ///   }
  ///   assert(i == 3);
  /// }
  ///
  /// \endcode
  template <typename T, typename U>
  struct ScopedAssignment
  {
    /// Create a ScopedAssignment.
    ///
    /// @param var The variable to assign.
    /// @param value The new value for the variable.
    ScopedAssignment(T& var, U value)
      : _var(&var)
      , _previous(std::move(var))
    {
      var = std::move(value);
    }
    /// Construct a ScopedAssignment from a moved ScopedAssignment.
    ///
    /// N.B. The variable stored by the given ScopedAssignment is lost.
    ScopedAssignment(ScopedAssignment&& assignment)
      : _var(assignment._var)
      , _previous(std::move(assignment._previous))
    {
      assignment._var = nullptr;
    }
    /// Destroy a ScopedAssignment, restoring the variable to its original
    /// value.
    ~ScopedAssignment()
    {
      if (this->_var)
        try
        {
          *this->_var = std::move(this->_previous);
        }
        catch (...)
        {
          ELLE_ABORT("exception in scoped assignment");
        }
    }

    ELLE_ATTRIBUTE(T*, var);
    ELLE_ATTRIBUTE(T, previous);
  };

  template <typename T, typename U>
  inline
  ScopedAssignment<T, U>
  scoped_assignment(T& var, U value)
  {
    return ScopedAssignment<T, U>(var, std::move(value));
  }
}

#endif
