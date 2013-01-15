#ifndef NUCLEUS_NEUTRON_CATALOG_HXX
# define NUCLEUS_NEUTRON_CATALOG_HXX

# include <elle/serialize/Serializer.hh>

# include <nucleus/Exception.hh>
# include <nucleus/neutron/Entry.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Catalog);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Catalog,
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
      std::shared_ptr<nucleus::neutron::Entry> entry{
        archive.template Construct<nucleus::neutron::Entry>().release()};

      // Compute the entry's footprint because the entry has been
      // created through the load constructor which does not have
      // the information to compute such a footprint.
      entry->footprint(elle::serialize::footprint(*entry));

      // Inject the entry in the catalog, taking care not to update
      // the state (the block must remain cleaned since deserialized).
      value._inject(entry);
    }

  ELLE_ASSERT(value.state() == nucleus::proton::State::clean);
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Catalog,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive << base_class<nucleus::proton::Value>(value);

  archive << static_cast<nucleus::neutron::Size>(value._container.size());

  for (auto& pair: value._container)
    {
      auto& entry = pair.second;

      archive << *entry;
    }
}

#endif
