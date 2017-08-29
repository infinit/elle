namespace elle
{
  namespace json
  {
    template <typename T>
    Json::Json(T&& v)
      : boost::any(std::forward<T>(v))
    {
      static_assert(!std::is_same<std::remove_cv_reference_t<T>, Json>::value,
                    "nesting Json inside a Json");
    }
  }
}
