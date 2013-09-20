#ifndef ELLE_FORMAT_GZIP_HH
# define ELLE_FORMAT_GZIP_HH

# include <iostream>
# include <fstream>
# include <zlib.h>

# include <boost/iostreams/filter/gzip.hpp>
# include <boost/iostreams/filtering_stream.hpp>

# include <elle/Buffer.hh>

namespace elle
{
  namespace format
  {
    namespace gzip
    {
      class StreamBuffer:
        public elle::StreamBuffer
      {
      public:
        StreamBuffer(std::ostream& underlying,
                     bool honor_flush,
                     Buffer::Size buffer_size);
        ~StreamBuffer();

        virtual WeakBuffer write_buffer();
        virtual WeakBuffer read_buffer();
        virtual void flush(Size size);

      private:
        void
        _send(int flush);
        ELLE_ATTRIBUTE(bool, honor_flush);
        ELLE_ATTRIBUTE(z_stream, z_stream);
        ELLE_ATTRIBUTE(std::ostream&, underlying);
        ELLE_ATTRIBUTE(elle::Buffer, buffer);
        ELLE_ATTRIBUTE(elle::Buffer, buffer_compression);
      };

      class Stream:
        public elle::IOStream
      {
      public:
        Stream(std::ostream& underlying,
               bool honor_flush,
               Buffer::Size buffer_size = 1 << 16);

      private:
        std::ostream& _underlying;
      };
    }
  }
}

#endif // !ELLE_FORMAT_GZIP_HH
