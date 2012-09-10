#ifndef  ELLE_SERIALIZE_STORECLASSVERSION_HH
# define ELLE_SERIALIZE_STORECLASSVERSION_HH

# include <type_traits>

namespace elle
{
  namespace serialize
  {

    ///
    /// Each specialized type gets its version stored along with the object
    /// itself. To change this behavior, you just have to define a
    /// specialization for each type that doesn't need to store its version.
    /// The specialization has to define a constant boolean named `value'.
    /// @see ELLE_SERIALIZE_NO_FORMAT for the macro shorcut.
    ///
    template<typename T> struct StoreFormat;

  }
}

/// Do not serialize any version for the given type.
#define ELLE_SERIALIZE_NO_FORMAT(Type)                                        \
  namespace elle { namespace serialize {                                      \
      template<> struct StoreFormat<Type> {                                   \
          static bool const value = false;                                    \
      };                                                                      \
  }}                                                                          \
  /**/

# include "StoreFormat.hxx"

#endif

