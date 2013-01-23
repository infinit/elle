#ifndef NUCLEUS_NEUTRON_ACCESS_HXX
# define NUCLEUS_NEUTRON_ACCESS_HXX

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

# include <nucleus/Exception.hh>
# include <nucleus/neutron/Record.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Access);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Access,
                          archive,
                          value,
                          version)
{
  nucleus::neutron::Size size;
  nucleus::neutron::Size i;

  enforce(version == 0);

  archive >> base_class<nucleus::proton::Value>(value);

  archive >> size;

  for (i = 0; i< size; i++)
    {
      std::shared_ptr<nucleus::neutron::Record> record{
        archive.template Construct<nucleus::neutron::Record>().release()};

      // Compute the record's footprint because the record has been
      // created through the load constructor which does not have
      // the information to compute such a footprint.
      record->_footprint = elle::serialize::footprint(*record);

      // Inject the record in the access, taking care not to update
      // the state (the block must remain cleaned since deserialized).
      value._inject(record);
    }

  ELLE_ASSERT(value.state() == nucleus::proton::State::clean);
  ELLE_ASSERT(value.footprint() == elle::serialize::footprint(value));
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Access,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive << base_class<nucleus::proton::Value>(value);

  archive << static_cast<nucleus::neutron::Size>(value._container.size());

  for (auto& pair: value._container)
    {
      auto& record = pair.second;

      archive << *record;
    }
}

#endif
