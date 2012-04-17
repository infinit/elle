#ifndef  ELLE_STANDALONE_REPORTSERIALIZER_HXX
# define ELLE_STANDALONE_REPORTSERIALIZER_HXX

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/standalone/Report.hh>

ELLE_SERIALIZE_SPLIT(elle::standalone::Report)

ELLE_SERIALIZE_SPLIT_SAVE(elle::standalone::Report,
                          archive,
                          value,
                          version)
{
  assert(version == 0);
  archive << static_cast<elle::Natural32>(value.container.size());
  auto it = value.container.begin(),
       end = value.container.end();
  for(; it != end; ++it)
    {
      auto entry = *it;
      assert(entry != nullptr);
      archive << entry->location
              << entry->time
              << entry->message;
    }
}

#endif

