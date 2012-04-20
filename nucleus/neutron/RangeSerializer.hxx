#ifndef  NUCLEUS_NEUTRON_RANGESERIALIZER_HXX
# define NUCLEUS_NEUTRON_RANGESERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/neutron/Range.hh>

namespace elle
{
  namespace serialize
  {
    template<typename T>
      struct ArchiveSerializer<nucleus::proton::Range<T>>
        : public SplitSerializer<nucleus::proton::Range<T>>
      {
        template<typename Archive> static void
          Save(Archive& ar,
               nucleus::proton::Range<T> const& value,
               unsigned int version)
          {
            assert(version == 0);

            archive << static_cast<Archive::ListSizeType>(value.container.size());

            auto it = value.container.begin(),
                 end = value.container.end();
            for (; it != end; ++it)
                archive << *(*it);
          }

        template<typename Archive> static void
          Load(Archive& ar,
               nucleus::proton::Range<T>& value,
               unsigned int version)
          {
            assert(version == 0);

            Archive::ListSizeType size;
            archive >> size;

            for (Archive::ListSizeType i = 0; i < size; ++i)
                value.container.push_back(archive.Construct<T>());
          }
      };
  }
}

#endif
