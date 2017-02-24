namespace elle
{
  template <typename T>
  T const&
  _elle_assert(T const& predicate,
               std::string const& message,
               char const* file,
               int line)
  {
    return _elle_assert(const_cast<T&>(predicate), message, file, line);
  }

  template <typename T>
  T&
  _elle_assert(T& predicate,
               std::string const& message,
               char const* file,
               int line)
  {
    if (predicate)
      return predicate;
    else
      _abort(message, file, line);
  }
}
