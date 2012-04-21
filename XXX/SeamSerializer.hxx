#ifndef  XXX_SEAMSERIALIZER_HXX
# define XXX_SEAMSERIALIZER_HXX

# include <cassert>

# include <XXX/NoduleSerializer.hxx>
# include <XXX/InletSerializer.hxx>

# include <XXX/Seam.hh>

namespace elle
{
  namespace serialize
  {

    template<typename V>
    struct ArchiveSerializer<nucleus::proton::Seam<V>>
      : public SplitSerializer<nucleus::proton::Seam<V>>
    {
    private:
      typedef nucleus::proton::Seam<V> Type;

    public:
      template<typename Archive> static void
        Save(Archive& ar, Type const& value, unsigned int version)
        {
          archive << static_cast<elle::proton::Nodule<V> const&>(value);
          Archive::ListSizeType size = value.container.size();
          archive << size;
          auto it = value.container.begin(),
               end = value.container.end();
          for (; it != end; ++it)
            {
              archive << it->first;
              archive << *(it->second);
            }
        }

      template<typename Archive> static void
        Load(Archive& ar, Type& value, unsigned int version)
        {
          archive >> static_cast<elle::proton::Nodule<V>&>(value);
          assert(value.container.size() == 0 && "Should not Load an already filled Seam<V>");
          Archive::ListSizeType size;
          archive >> size;
          for (Archive::ListSizeType i = 0; i < size; ++i)
            {
              typedef V::K k;
              archive >> k;
              auto v = archive.Construct<typename Type::I>();
              value.container[k] = v.get();
              v.release();
            }
        }
    };

  }
}

#endif
