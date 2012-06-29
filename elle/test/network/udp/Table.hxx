#ifndef  ELLE_TEST_NETWORK_UDP_TABLE_HXX
# define ELLE_TEST_NETWORK_UDP_TABLE_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/test/network/udp/Neighbour.hh>

# include <elle/test/network/udp/Table.hh>

ELLE_SERIALIZE_SPLIT(elle::test::Table)

ELLE_SERIALIZE_SPLIT_SAVE(elle::test::Table,
                          archive,
                          value,
                          version)
{
  assert(version == 0);
  uint32_t size = value.container.size();
  archive << size;
  auto it = value.container.begin(),
       end = value.container.end();
  for (; it != end; ++it)
    {
      archive << *(*it);
    }
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::test::Table,
                          archive,
                          value,
                          version)
{
  uint32_t size;
  archive >> size;

  for (size_t i = 0; i < size; ++i)
    {
      auto neighbour = new elle::test::Neighbour;

      archive >> *neighbour;

      value.container.push_back(neighbour);
    }
}

#endif
