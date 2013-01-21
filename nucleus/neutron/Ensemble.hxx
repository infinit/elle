#ifndef NUCLEUS_NEUTRON_ENSEMBLE_HXX
# define NUCLEUS_NEUTRON_ENSEMBLE_HXX

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

# include <nucleus/Exception.hh>
# include <nucleus/neutron/Fellow.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Ensemble);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Ensemble,
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
      std::shared_ptr<nucleus::neutron::Fellow> fellow{
        archive.template Construct<nucleus::neutron::Fellow>().release()};

      // Compute the fellow's footprint because the fellow has been
      // created through the load constructor which does not have
      // the information to compute such a footprint.
      fellow->_footprint = elle::serialize::footprint(*fellow);

      // Inject the fellow in the ensemble, taking care not to update
      // the state (the block must remain cleaned since deserialized).
      value._inject(fellow);
    }

  ELLE_ASSERT(value.state() == nucleus::proton::State::clean);
  ELLE_ASSERT(value.footprint() == elle::serialize::footprint(value));
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Ensemble,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive << base_class<nucleus::proton::Value>(value);

  archive << static_cast<nucleus::neutron::Size>(value._container.size());

  for (auto& pair: value._container)
    {
      auto& fellow = pair.second;

      archive << *fellow;
    }
}

#endif
