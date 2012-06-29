#ifndef  ELLE_NETWORK_HOST_HXX
# define ELLE_NETWORK_HOST_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/network/Host.hh>

ELLE_SERIALIZE_SPLIT(::QHostAddress);

ELLE_SERIALIZE_SPLIT_LOAD(::QHostAddress,
                          archive,
                          value,
                          version)
{
  assert(version == 0);
  std::string host;
  archive >> host;
  value.setAddress(host.c_str());
}

ELLE_SERIALIZE_SPLIT_SAVE(::QHostAddress,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  std::string host(value.toString().toStdString());

  archive << host;
}

ELLE_SERIALIZE_SIMPLE(elle::network::Host,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.type;
  archive & value.location;
}

#endif
