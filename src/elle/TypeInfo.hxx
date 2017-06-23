namespace elle
{
  template <typename T>
  TypeInfo
  type_info()
  {
    return &typeid(T);
  }

  template <typename T>
  TypeInfo
  type_info(T const& v)
  {
    return &typeid(v);
  }
}
