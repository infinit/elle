#ifndef NUCLEUS_NEUTRON_ATTRIBUTES_HXX
# define NUCLEUS_NEUTRON_ATTRIBUTES_HXX

# include <elle/serialize/Serializer.hh>

# include <nucleus/Exception.hh>
# include <nucleus/neutron/Trait.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Attributes);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Attributes,
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
      std::shared_ptr<nucleus::neutron::Trait> trait{
        archive.template Construct<nucleus::neutron::Trait>().release()};

      // Compute the trait's footprint because the trait has been
      // created through the load constructor which does not have
      // the information to compute such a footprint.
      trait->_footprint = elle::serialize::footprint(*trait);

      // Inject the trait in the attributes, taking care not to update
      // the state (the block must remain cleaned since deserialized).
      value._inject(trait);
    }

  ELLE_ASSERT(value.state() == nucleus::proton::State::clean);
  ELLE_ASSERT(value.footprint() == elle::serialize::footprint(value));
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Attributes,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive << base_class<nucleus::proton::Value>(value);

  archive << static_cast<nucleus::neutron::Size>(value._container.size());

  for (auto& pair: value._container)
    {
      auto& trait = pair.second;

      archive << *trait;
    }
}

#endif
