#ifndef ELLE_FORMAT_GZIP_HH
# define ELLE_FORMAT_GZIP_HH

# include <elle/Buffer.hh>
# include <elle/IOStream.hh>
# include <elle/compiler.hh>

namespace elle
{
  namespace format
  {
    namespace gzip
    {
      /// Stream wrapper that compresses to GZIP.
      ///
      /// Data written to the stream are compressed on the fly and written back
      /// to the wrapped stream.
      ///
      /// The honor_flush parameter enables to chose whether to force
      /// compression and writing on flush or not. Not doing so lets ZLIB choose
      /// whether to flush directly or keep compressing the next buffer if data
      /// have especially low entropy. Delaying writing this way can be a
      /// problem though if someone expects the data to be written after each
      /// flush. For instance, compressing complete data to a file on disk
      /// should set honor_flush to false to let ZLIB write a compressed block
      /// when it sees fit, but compressing network packet should set it to true
      /// to ensure compressed packets are sent immediately on flush() and not
      /// waiting to be compressed with the next ones.
      ///
      /// Decompression is not supported for now.
      class ELLE_API Stream
        : public elle::IOStream
      {
      public:
        /// Construct a Stream.
        ///
        /// \param underlying  The wrapped stream to write compressed data to.
        /// \param honor_flush Whether to force output when flushed.
        /// \param buffer_size The chunk size to compress data by.
        Stream(std::ostream& underlying,
               bool honor_flush,
               Buffer::Size buffer_size = 1 << 16);
      };
    }
  }
}

#endif // !ELLE_FORMAT_GZIP_HH
