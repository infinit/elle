#ifndef  ELLE_SERIALIZE_STORECLASSVERSION_HXX
# define ELLE_SERIALIZE_STORECLASSVERSION_HXX

namespace elle
{
  namespace serialize
  {

    template<typename T> struct StoreFormat
    { static bool const value = true; };

    template<typename T> struct StoreFormat<T&>
    { static bool const value = StoreFormat<T>::value; };

    template<typename T> struct StoreFormat<T const>
    { static bool const value = StoreFormat<T>::value; };

    template<typename T> struct StoreFormat<T volatile>
    { static bool const value = StoreFormat<T>::value; };

    template<typename T> struct StoreFormat<T*>
    { static_assert(std::is_same<T,T>::value, "Should not happen !"); };

  }
}

#endif

