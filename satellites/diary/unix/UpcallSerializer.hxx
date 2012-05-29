#ifndef  SATELLITES_DIARY_UNIX_UPCALLSERIALIZER_HXX
# define SATELLITES_DIARY_UNIX_UPCALLSERIALIZER_HXX

# include <cassert>

# include <elle/utility/BufferSerializer.hxx>

# include <satellites/diary/unix/Upcall.hh>

ELLE_SERIALIZE_SIMPLE(satellite::unix::Upcall,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.operation;
  archive & value.result;
  archive & value._inputs;
  archive & value._outputs;
}

#endif
