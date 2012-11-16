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
  elle::Natural32 size;
  elle::Natural32 i;

  enforce(version == 0);

  archive >> base_class<nucleus::proton::Node>(value);

  archive >> size;

  for (i = 0; i< size; i++)
    {
      auto entry =
        archive.template Construct<nucleus::neutron::Entry>();

      // Compute the entry's footprint because the entry proper
      // constructor has not been called. Instead, the default
      // constructor has been used before deserializing the entry.
      entry->footprint(elle::serialize::footprint(*entry));

      std::pair<typename nucleus::neutron::Catalog::Iterator,
                elle::Boolean> result;

      if (value._container.find(entry->name()) != value._container.end())
        throw nucleus::Exception(
          elle::sprintf("the name '%s' seems to have already been recorded.",
                        entry->name()));

      result = value._container.insert(
        std::pair<const elle::String, nucleus::neutron::Entry*>(
          entry->name(), entry.get()));

      if (result.second == false)
        throw nucleus::Exception("unable to insert the entry in "
                                 "the container.");

      // Update the catalog's footprint.
      value.footprint(value.footprint() + entry->footprint());

      entry.release();
    }
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Catalog,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive << base_class<nucleus::proton::Node>(value);

  archive << static_cast<elle::Natural32>(value._container.size());

  auto it = value._container.begin();
  auto end = value._container.end();

  for (; it != end; ++it)
    archive << *(it->second);
}

#endif
