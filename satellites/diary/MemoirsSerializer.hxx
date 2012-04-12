#ifndef SATELLITES_DIARY_MEMOIRSSERIALIZER_HXX
# define SATELLITES_DIARY_MEMOIRSSERIALIZER_HXX

# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

# include "Memoirs.hh"

ELLE_SERIALIZE_SPLITTED(satellite::Memoirs)

ELLE_SERIALIZE_SPLITTED_SAVE(satellite::Memoirs,
                             archive,
                             memoirs,
                             version)
{
  (void) version;

  // XXX uses uint64_t to store the size
  // It means that a 32bit platform might not read successfully a archive of
  // Memoirs.
  uint64_t size = memoirs.archive.size;
  archive << size;

  archive.SaveBinary(memoirs.archive.contents, size);
}

ELLE_SERIALIZE_SPLITTED_LOAD(satellite::Memoirs,
                             archive,
                             memoirs,
                             version)
{
  (void) version;

  uint64_t size;
  archive >> size;
  auto ptr = static_cast<elle::Byte*>(::malloc(size));
  if (ptr == nullptr)
    throw std::bad_alloc();
  try
    {
      archive.LoadBinary(ptr, size);
    }
  catch (std::exception const&)
    {
      ::free(ptr);
      throw;
    }
}

#endif
