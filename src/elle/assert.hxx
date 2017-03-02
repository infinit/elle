namespace elle
{
  template <typename T>
  decltype(auto)
  _elle_assert(T&& predicate,
               std::string const& message,
               char const* file,
               int line)
  {
    if (predicate)
      return std::forward<T>(predicate);
    else
      _abort(message, file, line);
  }
}
