namespace elle
{
  template <typename T>
  TypeInfo
  type_info()
  {
    return TypeInfo(&typeid(T));
  }

  template <typename T>
  TypeInfo
  type_info(T const& v)
  {
    return TypeInfo(&typeid(v));
  }
}
