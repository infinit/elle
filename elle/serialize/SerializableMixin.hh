#ifndef  ELLE_SERIALIZE_SERIALIZABLEMIXIN_HH
# define ELLE_SERIALIZE_SERIALIZABLEMIXIN_HH

# include <elle/serialize/Serializable.hh>

namespace elle
{
  namespace serialize
  {

    template<
        typename T
      , template<ArchiveMode> class Archive = BinaryArchive
    >
    class SerializableMixin
      : public Serializable<Archive>
    {
    public:
      typedef typename Serializable<Archive>::InputArchive   InputArchive;
      typedef typename Serializable<Archive>::OutputArchive  OutputArchive;

    public:
      virtual void LoadFrom(InputArchive& archive);
      virtual void SaveTo(OutputArchive& archive) const;
    };

  }
}

//XXX
//# include <elle/serialize/SerializableMixin.hxx>

#endif /* ! SERIALIZABLEMIXIN_HH */


