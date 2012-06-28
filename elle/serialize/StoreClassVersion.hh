#ifndef  STORECLASSVERSION_HH
# define STORECLASSVERSION_HH

# include <type_traits>

namespace elle
{
  namespace serialize
  {

    ///
    /// Each specialized type gets its version stored along with the object
    /// itself. To change this behavior, you just have to define a specialization
    /// for each type that doesn't need to store its version.
    ///
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

/// Do not serialize any version for the given type.
#define ELLE_SERIALIZE_NO_VERSION(Type)                                       \
  namespace elle { namespace serialize {                                      \
      template<> struct StoreClassVersion<Type> {                             \
          static bool const value = false;                                    \
      };                                                                      \
  }}                                                                          \
  /**/

#endif

