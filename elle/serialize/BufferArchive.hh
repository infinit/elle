#ifndef  ELLE_SERIALIZE_BUFFERARCHIVE_HH
# define ELLE_SERIALIZE_BUFFERARCHIVE_HH

# include <elle/utility/fwd.hh>

# include <elle/utility/BufferStream.hh>

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
    public:
      typedef typename BaseClass::StreamType  StreamType;

    private:
      StreamType    _stream;

    public:
      BufferArchive(StreamType& stream);
      BufferArchive(BufferArchive const& other);
      BufferArchive(typename StreamType::BufferType& buffer);
      template<typename T>
        BufferArchive(StreamType& stream, T& value)
          : BaseClass(stream)
          , _stream(stream)
        { *this & value; }


    public:
      StreamType& Stream() { return _stream; }
    };

    typedef BufferArchive<ArchiveMode::Input>  InputBufferArchive;
    typedef BufferArchive<ArchiveMode::Output> OutputBufferArchive;
  }
}


#endif /* ! BUFFERARCHIVE_HH */


