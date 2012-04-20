#ifndef  SATELLITES_DIARY_UNIX_UPCALLSERIALIZER_HXX
# define SATELLITES_DIARY_UNIX_UPCALLSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <satellites/diary/unix/Upcall.hh>

ELLE_SERIALIZE_SIMPLE(satellites::diary::unix::Upcall,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.operation;
  // XXX
  //archive & value.inputs;
  //archive & value.outputs;
  archive & value.result;
}

#endif
