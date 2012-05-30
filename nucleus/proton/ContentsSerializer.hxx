#ifndef  NUCLEUS_PROTON_CONTENTSSERIALIZER_HXX
# define NUCLEUS_PROTON_CONTENTSSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/cryptography/CipherSerializer.hxx>
# include <nucleus/proton/ContentHashBlockSerializer.hxx>

# include <nucleus/proton/Contents.hh>

namespace elle
{
  namespace serialize
  {

    template<typename T>
      struct ArchiveSerializer<nucleus::proton::Contents<T>>
        : public SplitSerializer<nucleus::proton::Contents<T>>
      {
      private:
        typedef nucleus::proton::Contents<T>        Type;
        typedef nucleus::proton::ContentHashBlock   Super;

      public:
        template<typename Archive> static void
          Save(Archive& ar,
               Type const& value,
               unsigned int version)
          {
            if (value.cipher == nullptr)
              throw std:runtime_error("Cannot encrypt Contents without cipher");
            archive << static_cast<Super&>(value);
            archive << *(value.cipher);
          }

        template<typename Archive> static void
          Load(Archive& ar,
               Type& value,
               unsigned int version)
          {
            delete value.cipher;
            value.cipher = nullptr;

            archive >> static_cast<Super&>(value);
            value.cipher = archive.Construct<Type>().release();
          }
      };

  }
}

#endif
