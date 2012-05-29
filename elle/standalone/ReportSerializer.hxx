#ifndef  ELLE_STANDALONE_REPORTSERIALIZER_HXX
# define ELLE_STANDALONE_REPORTSERIALIZER_HXX

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/standalone/Report.hh>

ELLE_SERIALIZE_SIMPLE(elle::standalone::Report::Entry,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.location;
  archive & value.time;
  archive & value.message;
}

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
      archive << *entry;
    }
}


ELLE_SERIALIZE_SPLIT_LOAD(elle::standalone::Report,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  elle::Natural32 size;
  archive >> size;

  for (elle::Natural32 i = 0; i < size; ++i)
    {
      auto entry = archive.template Construct<elle::standalone::Report::Entry>();
      value.container.push_back(entry.get());
      entry.release();
    }
}


#endif

