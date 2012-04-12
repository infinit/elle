#ifndef ELLE_UTILITY_SETTINGSSERIALIZER_HXX
# define ELLE_UTILITY_SETTINGSSERIALIZER_HXX

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/format/ini/Section.hxx>

# include "Settings.hh"

//namespace elle { namespace serialize {
//
//    template<>
//      struct ArchiveSerializer<elle::utility::Settings>
//        : public BaseArchiveSerializer<elle::utility::Settings>
//      {
//        template<typename Archive>
//          static void Serialize(Archive& archive,
//                                elle::utility::Settings& value,
//                                unsigned int)
//          {
//          }
//      };
//
//}}

ELLE_SERIALIZE_SIMPLE_SERIALIZER(elle::utility::Settings, archive, value, version)
{
  (void) version;

}

#endif
