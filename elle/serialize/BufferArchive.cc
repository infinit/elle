
#include <elle/utility/BufferStream.hh>
#include <elle/serialize/BufferArchive.hh>

using namespace elle::serialize;

template<ArchiveMode mode>
BufferArchive<mode>::BufferArchive(StreamType const& stream)
  : BaseClass(this->_stream)
  , _stream(stream)
{}

template<ArchiveMode mode>
BufferArchive<mode>::BufferArchive(BufferArchive const& other)
  : BaseClass(this->_stream)
  , _stream(other->_stream)
{}
