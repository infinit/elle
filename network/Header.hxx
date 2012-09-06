#ifndef  ELLE_NETWORK_HEADER_HXX
# define ELLE_NETWORK_HEADER_HXX

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SPLIT(elle::network::Header);

ELLE_SERIALIZE_SPLIT_SAVE(elle::network::Header,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.tag;
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::network::Header,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.tag;
}

#endif
