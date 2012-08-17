#ifndef NUCLEUS_NEUTRON_ENSEMBLE_HXX
# define NUCLEUS_NEUTRON_ENSEMBLE_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/serialize/Pointer.hh>

# include <nucleus/neutron/Fellow.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Ensemble);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Ensemble,
                          archive,
                          value,
                          version)
{
  typename Archive::SequenceSizeType size;

  assert(version == 0);

  archive >> static_cast<nucleus::proton::ContentHashBlock&>(value);
  archive >> size;

  for (typename Archive::SequenceSizeType i = 0; i < size; ++i)
    value._container.push_back(
      archive.template Construct<nucleus::neutron::Fellow>().release());
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Ensemble,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  archive << static_cast<nucleus::proton::ContentHashBlock const&>(value);
  archive <<
    static_cast<typename Archive::SequenceSizeType>(value._container.size());

  auto iterator = value._container.begin();
  auto end = value._container.end();

  for (; iterator != end; ++iterator)
    archive << *(*iterator);
}

#endif
