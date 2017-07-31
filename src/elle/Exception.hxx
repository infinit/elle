namespace elle
{
  template <typename T>
  void
  throw_with_nested(T&& e)
  {
    try
    {
      std::rethrow_exception(std::current_exception());
    }
    catch (elle::Exception& e)
    {
      e._backtrace.strip_base(Backtrace::current());
    }
    catch (...)
    {}
    std::throw_with_nested(std::move(e));
  }
}
