#ifndef NUCLEUS_NEUTRON_DATA_HXX
# define NUCLEUS_NEUTRON_DATA_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Data);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Data,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive >> base_class<nucleus::proton::Value>(value);

  archive >> value._offset;
  archive >> value._buffer;

  // Set the footprint to its initial plus the amount of content.
  value.footprint(value.footprint() + value._buffer.size());

  ELLE_ASSERT(value.state() == nucleus::proton::State::clean);
  ELLE_ASSERT(value.footprint() == elle::serialize::footprint(value));
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Data,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive << base_class<nucleus::proton::Value>(value);

  archive << value._offset;
  archive << value._buffer;
}

#endif
