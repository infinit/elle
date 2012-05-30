#ifndef  NUCLEUS_NEUTRON_RANGESERIALIZER_HXX
# define NUCLEUS_NEUTRON_RANGESERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/neutron/Range.hh>


ELLE_SERIALIZE_SPLIT_T1(nucleus::neutron::Range);

ELLE_SERIALIZE_SPLIT_T1_LOAD(nucleus::neutron::Range,
                             archive,
                             value,
                             version)
{
  typedef typename Archive::SequenceSizeType SizeType;
  assert(version == 0);

  SizeType size;
  archive >> size;

  for (SizeType i = 0; i < size; ++i)
    value.container.push_back(
        archive.template Construct<T1>().release()
    );
}

ELLE_SERIALIZE_SPLIT_T1_SAVE(nucleus::neutron::Range,
                             archive,
                             value,
                             version)
{
  typedef typename Archive::SequenceSizeType SizeType;
  assert(version == 0);
  archive << static_cast<SizeType>(value.container.size());

  auto it = value.container.begin(),
       end = value.container.end();
  for (; it != end; ++it)
    archive << *(*it);
}


#endif
