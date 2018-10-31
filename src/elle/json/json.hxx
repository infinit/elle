#include <elle/make-vector.hh>

namespace elle
{
  namespace json
  {
    template <typename T>
    Json::Json(std::vector<T> v,
               std::enable_if_t<is_jsonifiable<T>(), bool>)
      : boost::any(elle::make_vector(v, [] (T const& e) { return Json(e); }))
    {}

    template <typename T>
    Json::Json(T&& v)
      : boost::any(std::forward<T>(v))
    {
      static_assert(!std::is_same<std::remove_cv_reference_t<T>, Json>::value,
                    "nesting Json inside a Json");
    }
  }
}
