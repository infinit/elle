#include <elle/format/base64.hh>
#include <elle/format/base64url.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.format.base64url")

namespace elle
{
  namespace format
  {
    namespace base64url
    {
      class RewriteStreamBuffer:
        public PlainStreamBuffer
      {
      public:
        RewriteStreamBuffer(std::iostream& backend):
          _backend(backend)
        {}

        Size
        read(char* buffer,
             Size size)
        {
          this->_backend.read(buffer, size);
          size = this->_backend.gcount();
          for (elle::Byte& c: elle::WeakBuffer(buffer, size))
          {
            if (c == '-')
              c = '+';
            else if (c == '_')
              c = '/';
          }
          return size;
        }

        void
        write(char* buffer,
              Size size)
        {
          for (elle::Byte& c: elle::WeakBuffer(buffer, size))
          {
            if (c == '+')
              c = '-';
            else if (c == '/')
              c = '_';
          }
          this->_backend.write(buffer, size);
        }

      private:
        ELLE_ATTRIBUTE(std::iostream&, backend);
      };

      Stream::Stream(std::iostream& underlying):
        IOStream(this->_buffer = new base64::StreamBuffer(this->_rewritter)),
        _rewritter(this->_rewrite_buffer = new RewriteStreamBuffer(underlying))
      {}

      Stream::~Stream()
      {
        ELLE_TRACE_SCOPE("%s: finalize encoding", *this->_buffer);
        this->flush();
        this->_buffer->finalize();
      }
    }
  }
}
