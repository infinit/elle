#ifndef  ELLE_CRYPTOGRAPHY_LARGESERIALIZER_HXX
# define ELLE_CRYPTOGRAPHY_LARGESERIALIZER_HXX

# include <stdexcept>

# include <elle/types.hh>

# include <elle/serialize/ArchiveSerializer.hxx>

// XXX The type elle::Large may not have a reasonable maximum size, in which
// case we'll have to change the initial alloc system (and remove these
// macros).

// serialization of a Large max size
# define ELLE_LARGE_MAX_BIN_SIZE    16384

// type used to store the size
# define ELLE_LARGE_BIN_SIZE_TYPE   uint16_t

# define __ASSERT_ELLE_LARGE_SIZE_TYPE_IS_VALID(Type)                         \
  static_assert(                                                              \
      ELLE_LARGE_MAX_BIN_SIZE <= static_cast<Type>(-1),                       \
      "The size cannot be stored in a " #Type                                 \
  )                                                                           \

# define _ASSERT_ELLE_LARGE_SIZE_TYPE_IS_VALID()                              \
  __ASSERT_ELLE_LARGE_SIZE_TYPE_IS_VALID(ELLE_LARGE_BIN_SIZE_TYPE)            \

ELLE_SERIALIZE_SPLIT(elle::Large)

ELLE_SERIALIZE_SPLIT_LOAD(elle::Large,
                          archive,
                          n,
                          version)
{
  int i = 0;
  _ASSERT_ELLE_LARGE_SIZE_TYPE_IS_VALID();

  char unsigned tab[ELLE_LARGE_MAX_BIN_SIZE];
  ELLE_LARGE_BIN_SIZE_TYPE size;
  archive >> size;
  if (size > ELLE_LARGE_MAX_BIN_SIZE)
    throw std::runtime_error("Cannot load large number that big");
  archive.LoadBinary(tab, size);
  std::cout << "__ > size = " << (size_t) size << std::endl;
  ::BN_bin2bn(tab, size, &n);
  std::cout << "__ > " << ++i << std::endl;
}

ELLE_SERIALIZE_SPLIT_SAVE(elle::Large,
                          archive,
                          n,
                          version)
{
  _ASSERT_ELLE_LARGE_SIZE_TYPE_IS_VALID();

  unsigned char tab[ELLE_LARGE_MAX_BIN_SIZE];
  size_t size = BN_num_bytes(&n);

  assert(size < ELLE_LARGE_MAX_BIN_SIZE);

  ::BN_bn2bin(&n, tab);

  archive << static_cast<ELLE_LARGE_BIN_SIZE_TYPE>(size);
  archive.SaveBinary(tab, size);
}

# undef _ASSERT_ELLE_LARGE_SIZE_TYPE_IS_VALID
# undef __ASSERT_ELLE_LARGE_SIZE_TYPE_IS_VALID

#endif
