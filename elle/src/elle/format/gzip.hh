#ifndef ELLE_FORMAT_GZIP_HH
# define ELLE_FORMAT_GZIP_HH

# include <iostream>
# include <fstream>

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
        StreamBuffer(std::ostream& underlying);

        virtual WeakBuffer write_buffer();
        virtual WeakBuffer read_buffer();
        virtual void flush(Size size);

      private:
        std::ostream& _underlying;
        elle::Buffer _buffer;
      };

      class Stream:
        public elle::IOStream
      {
      public:
        Stream(std::ostream& underlying);

      private:
        std::ostream& _underlying;
      };
    }
  }
}

#endif // !ELLE_FORMAT_GZIP_HH
