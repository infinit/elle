#ifndef  ELLE_SERIALIZE_SERIALIZABLEMIXIN_HXX
# define ELLE_SERIALIZE_SERIALIZABLEMIXIN_HXX

# include <elle/serialize/SerializableMixin.hh>

namespace elle
{
  namespace serialize
  {

    template<
        typename T
      , template<ArchiveMode> class Archive
    >
    void SerializableMixin<T, Archive>::LoadFrom(InputArchive& archive)
    {
      archive >> static_cast<T&>(*this);
    }

    template<
        typename T
      , template<ArchiveMode> class Archive
    >
    void SerializableMixin<T, Archive>::SaveTo(OutputArchive& archive) const
    {
      archive << static_cast<T const&>(*this);
    }

  }
}

#endif
