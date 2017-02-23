#ifndef ELLE_TYPE_INFO_HXX
# define ELLE_TYPE_INFO_HXX

namespace elle
{
  template <typename T>
  inline
  TypeInfo
  type_info()
  {
    return TypeInfo(&typeid(T));
  }

  template <typename T>
  inline
  TypeInfo
  type_info(T const& v)
  {
    return TypeInfo(&typeid(v));
  }
}

#endif
