#ifndef  LUNE_MAPSERIALIZER_HXX
# define LUNE_MAPSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <lune/Map.hh>

ELLE_SERIALIZE_SPLIT_T1(lune::Map);

ELLE_SERIALIZE_SPLIT_T1_SAVE(lune::Map,
                             archive,
                             value,
                             version)
{
  assert(version == 0);

  archive << static_cast<Archive::ListSizeType>(value.container.size());

  auto it = value.container.begin(),
       end = value.container.end();
  for (; it != end; ++it)
    {
      lune::Map<T1>::Entry* entry = *it;
      archive << entry.name << entry.value;
    }
}


ELLE_SERIALIZE_SPLIT_T1_LOAD(lune::Map,
                             archive,
                             value,
                             version)
{
  assert(version == 0);

  Archive::ListSizeType size;
  archive >> size;

  for (ArchiveListSizeType i = 0; i < size; ++i)
    {
      auto entry = std::unique_ptr<lune::Map<T1>::Entry>(
          new lune::Map<T1>::Entry()
      );
      archive >> entry->name >> entry->value;
      value.container.push_back(entry.get());
      entry.release();
    }
}

#endif
