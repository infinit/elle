#ifndef NUCLEUS_NEUTRON_CATALOG_HXX
# define NUCLEUS_NEUTRON_CATALOG_HXX

# include <elle/serialize/Serializer.hh>

# include <nucleus/Exception.hh>

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

      // Compute the entry's footprint because the entry proper
      // constructor has not been called. Instead, the default
      // constructor has been used before deserializing the entry.
      entry->footprint(elle::serialize::footprint(*entry));

      if (value._container.find(entry->name()) != value._container.end())
        throw nucleus::Exception("the entry's name '%s' seems to already "
                                 "exist", entry->name());

      auto result =
        value._container.insert(
          std::pair<elle::String const,
                    std::shared_ptr<nucleus::neutron::Entry>>{
            entry->name(), entry});

      if (result.second == false)
        throw Exception("unable to insert the entry the container");

      // Update the catalog's footprint.
      value.footprint(value.footprint() + entry->footprint());
    }
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
