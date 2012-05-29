
#include <elle/utility/BufferStream.hh>
#include <elle/serialize/BufferArchive.hh>

using namespace elle::serialize;

template<ArchiveMode mode>
BufferArchive<mode>::BufferArchive(StreamType& stream)
  : BaseClass(this->_stream)
  , _stream(stream)
{}

template<ArchiveMode mode>
BufferArchive<mode>::BufferArchive(BufferArchive const& other)
  : BaseClass(this->_stream)
  , _stream(other._stream)
{}

template<ArchiveMode mode>
BufferArchive<mode>::BufferArchive(typename StreamType::BufferType& buffer)
  : BaseClass(this->_stream)
  , _stream(buffer)
{}


template class BufferArchive<ArchiveMode::Input>;
template class BufferArchive<ArchiveMode::Output>;
