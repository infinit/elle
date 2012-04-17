#ifndef  ELLE_SERIALIZE_FWD_HH
# define ELLE_SERIALIZE_FWD_HH

# include <elle/serialize/ArchiveMode.hh>

namespace elle
{
  namespace serialize
  {

    typedef char                                    DefaultCharType;
    template<ArchiveMode, typename> struct          DefaultStreamTypeSelect;

    template<ArchiveMode mode> class                BinaryArchive;
    typedef BinaryArchive<ArchiveMode::Output>      OutputBinaryArchive;
    typedef BinaryArchive<ArchiveMode::Input>       InputBinaryArchive;

    template<ArchiveMode mode> class                Base64Archive;
    typedef Base64Archive<ArchiveMode::Output>      OutputBase64Archive;
    typedef Base64Archive<ArchiveMode::Input>       InputBase64Archive;

    template<ArchiveMode mode> class                HexadecimalArchive;
    typedef HexadecimalArchive<ArchiveMode::Output> OutputHexadecimalArchive;
    typedef HexadecimalArchive<ArchiveMode::Input>  InputHexadecimalArchive;

    template<ArchiveMode mode> class                JSONArchive;
    class                                           OutputJSONArchive;
    class                                           InputJSONArchive;

    template<ArchiveMode mode> class                BufferArchive;
    typedef BufferArchive<ArchiveMode::Input>       InputBufferArchive;
    typedef BufferArchive<ArchiveMode::Output>      OutputBufferArchive;

    template<typename T> struct                     ArchiveSerializer;
  }
}

#endif


