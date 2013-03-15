#ifndef  ELLE_SERIALIZE_STATICFORMAT_HH
# define ELLE_SERIALIZE_STATICFORMAT_HH

namespace elle
{
  namespace serialize
  {

    ///
    /// This structure is used to determine the version of a class
    /// serialization.  The version defaults to 0, but you may want to
    /// specialize this class to set a superior version number.
    /// @see ELLE_SERIALIZE_STATIC_FORMAT
    ///
    template <typename T>
    struct StaticFormat
    {
      static unsigned int const version = 0;
    };

  }
}

/// Convenient macro to declare the version of a serialized class
# define ELLE_SERIALIZE_STATIC_FORMAT(__type, __version)                      \
  namespace elle { namespace serialize {                                      \
    template <> struct StaticFormat<__type>                                   \
    {                                                                         \
      static unsigned int const version = __version;                          \
    };                                                                        \
  }}                                                                          \
/**/

#endif

