#include "stream.hh"

#include <elle/Exception.hh>

#include <iostream>

namespace elle
{
  namespace serialize
  {
    namespace detail
    {

      void
      stream_write(std::ostream& out,
                   void const* data,
                   std::streamsize const size)
      {
        out.write(static_cast<char const*>(data), size);
        if (out.fail() or out.bad())
          throw elle::Exception{"Couldn't write to the stream"};
      }

      void
      stream_read(std::istream& in,
                  void* data,
                  std::streamsize const size)
      {
        in.read(static_cast<char*>(data), size);
        if (in.fail() or in.bad())
          throw elle::Exception{"Couldn't read from the stream"};
      }

    }
  }
}
