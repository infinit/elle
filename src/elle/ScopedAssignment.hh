#ifndef ELLE_SCOPED_ASSIGNMENT_HH
# define ELLE_SCOPED_ASSIGNMENT_HH

namespace elle
{
  template <typename T, typename U>
  struct ScopedAssignment
  {
    ScopedAssignment(T& var, U value)
      : _var(&var)
      , _previous(std::move(var))
    {
      var = std::move(value);
    }

    ScopedAssignment(ScopedAssignment&& assignment)
      : _var(assignment._var)
      , _previous(std::move(assignment._previous))
    {
      assignment._var = nullptr;
    }

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
