namespace elle
{
  namespace detail
  {
    void on_abort(AbortCallback fun);
  }

  template <typename Fun>
  auto on_abort(Fun&& fun)
    -> decltype(AbortCallback(fun), void())
  {
    detail::on_abort(AbortCallback{std::forward<Fun>(fun)});
  }

  template <typename T>
  decltype(auto)
  _elle_assert(T&& predicate,
               std::string const& message,
               char const* file, int line)
  {
    if (predicate)
      return std::forward<T>(predicate);
    else
      _abort(message, file, line);
  }
}
