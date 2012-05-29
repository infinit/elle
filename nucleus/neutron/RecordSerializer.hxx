#ifndef  NUCLEUS_NEUTRON_RECORDSERIALIZER_HXX
# define NUCLEUS_NEUTRON_RECORDSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <nucleus/neutron/SubjectSerializer.hxx>
# include <nucleus/neutron/TokenSerializer.hxx>

# include <nucleus/neutron/Record.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Record,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.subject;
  archive & value.permissions;
  archive & value.token;
}

#endif
