#ifndef  ELLE_SERIALIZE_BUFFERARCHIVE_HH
# define ELLE_SERIALIZE_BUFFERARCHIVE_HH

# include <elle/utility/fwd.hh>

# include <elle/serialize/BinaryArchive.hh>

namespace elle
{
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

    } // !namespace detail

    template<ArchiveMode mode>
    class BufferArchive
      : public BaseBinaryArchive<
          mode
        , BufferArchive<mode>
        , DefaultCharType
        , detail::BufferStreamSelect
      >
    {
    private:
      typedef BaseBinaryArchive<
          mode
        , BufferArchive<mode>
        , DefaultCharType
        , detail::BufferStreamSelect
      >                                       BaseClass;
      typedef typename BaseClass::StreamType  StreamType;

    private:
      StreamType    _stream;

    public:
      BufferArchive(StreamType const& stream);
      BufferArchive(BufferArchive const& other);

    public:
      StreamType& Stream() { return _stream; }
    };

    typedef BufferArchive<ArchiveMode::Input>  InputBufferArchive;
    typedef BufferArchive<ArchiveMode::Output> OutputBufferArchive;
  }
}


#endif /* ! BUFFERARCHIVE_HH */


