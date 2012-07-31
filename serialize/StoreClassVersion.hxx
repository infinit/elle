#ifndef  ELLE_SERIALIZE_STORECLASSVERSION_HXX
# define ELLE_SERIALIZE_STORECLASSVERSION_HXX

namespace elle
{
  namespace serialize
  {

    template<typename T> struct StoreClassVersion
    { static bool const value = true; };

    template<typename T> struct StoreClassVersion<T&>
    { static bool const value = StoreClassVersion<T>::value; };

    template<typename T> struct StoreClassVersion<T const>
    { static bool const value = StoreClassVersion<T>::value; };

    template<typename T> struct StoreClassVersion<T volatile>
    { static bool const value = StoreClassVersion<T>::value; };

    template<typename T> struct StoreClassVersion<T*>
    { static_assert(std::is_same<T,T>::value, "Should not happen !"); };

  }
}

#endif

