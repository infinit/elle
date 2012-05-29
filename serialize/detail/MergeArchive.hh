#ifndef ELLE_SERIALIZE_DETAIL_MERGGEARCHIVE_HH
# define ELLE_SERIALIZE_DETAIL_MERGGEARCHIVE_HH

# include <boost/mpl/if.hpp>

# include <elle/serialize/ArchiveMode.hh>

namespace elle { namespace serialize { namespace detail {

    template<
        typename InputArchive
      , typename OutputArchive
      , elle::serialize::ArchiveMode mode
    >
      struct _ArchiveSelector
      {
        typedef typename boost::mpl::if_c<
            mode == elle::serialize::ArchiveMode::Input
          , InputArchive
          , OutputArchive
        >::type type;
      };

}}}

/// Utility macro to declare a templated generic archive from Splitted classes
#define ELLE_SERIALIZE_MERGE_ARCHIVES(class_name, InputArchive, OutputArchive)\
  template<                                                                   \
        ArchiveMode mode_                                                     \
  >                                                                           \
    class class_name                                                          \
    : public elle::serialize::detail::_ArchiveSelector<                       \
        InputArchive                                                          \
      , OutputArchive                                                         \
      , mode_                                                                 \
    >::type                                                                   \
    {                                                                         \
    public:                                                                   \
      static ArchiveMode const mode = mode_;                                  \
    private:                                                                  \
      typedef typename elle::serialize::detail::_ArchiveSelector<             \
          InputArchive                                                        \
        , OutputArchive                                                       \
        , mode                                                                \
      >::type BaseClass;                                                      \
      static_assert(                                                          \
          mode == BaseClass::mode, "in/out mismatch in archive selector"      \
      );                                                                      \
    public:                                                                   \
      class_name(typename BaseClass::StreamType& stream) : BaseClass(stream)  \
        {}                                                                    \
      template<typename T>                                                    \
      class_name(typename BaseClass::StreamType& stream,                      \
                 T const& value) : BaseClass(stream, value)                   \
        {}                                                                    \
    }                                                                         \

#endif /* ! SPLITARCHIVE_HH */

