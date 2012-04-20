#ifndef  NUCLEUS_NEUTRON_TOKENSERIALIZER_HXX
# define NUCLEUS_NEUTRON_TOKENSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/cryptography/CodeSerializer.hxx>

# include <nucleus/neutron/Token.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Token);

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Token,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  if (value.code == nullptr)
    {
      archive << true;
      archive << *value.code;
    }
  else
    {
      archive << false;
    }
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Token,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  delete value.code;
  value.code = nullptr;

  bool has_token;
  archive >> has_token;

  if (has_token)
    value.code = archive.Construct<nucleus::neutron::Token>().release();
}

#endif
