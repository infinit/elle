#ifndef  ELLE_SERIALIZE_BUFFERARCHIVE_HH
# define ELLE_SERIALIZE_BUFFERARCHIVE_HH

# include <elle/serialize/BinaryArchive.hh>
# include <elle/serialize/detail/MergeArchive.hh>

namespace elle
{
  namespace utility
  {
    class InputBufferStream;
    class OutputBufferStream;
  }

  namespace serialize
  {

    namespace detail
    {
      template<ArchiveMode mode, typename CharType>
        struct BufferStreamSelect;

      template<>
        struct BufferStreamSelect<ArchiveMode::Input, DefaultCharType>
          { typedef elle::utility::InputBufferStream type; };

      template<>
        struct BufferStreamSelect<ArchiveMode::Output, DefaultCharType>
          { typedef elle::utility::OutputBufferStream type; };


    }

    typedef BaseBinaryArchive<
        ArchiveMode::Input
      , DefaultCharType
      , detail::BufferStreamSelect
    >                               InputBufferArchive;

    typedef BaseBinaryArchive<
        ArchiveMode::Output
      , DefaultCharType
      , detail::BufferStreamSelect
    >                               OutputBufferArchive;

    ELLE_SERIALIZE_MERGE_ARCHIVES(
        BufferArchive,
        InputBufferArchive,
        OutputBufferArchive
    );

  }
}


#endif /* ! BUFFERARCHIVE_HH */


