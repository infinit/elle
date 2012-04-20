#ifndef  NUCLEUS_PROTON_ADDRESSSERIALIZER_HXX
# define NUCLEUS_PROTON_ADDRESSSERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/DigestSerializer.hxx>

# include <nucleus/proton/Address.hh>

ELLE_SERIALIZE_SPLIT(nucleus::proton::Address);

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::proton::Address,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  if (value.digest != nullptr)
    {
      archive << true
              << value.family
              << value.component
              << *(value.digest);
    }
  else
    {
      archive << false;
    }
}

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::proton::Address,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  delete value.digest;
  value.digest = nullptr;

  bool has_digest;
  archive >> has_digest;

  if (!has_digest)
    return;

  archive >> value.family >> value.component;
  value.digest = archive.construct<elle::cryptography::Digest>().release();
}

#endif
