#ifndef  ELLE_SERIALIZE_DETAIL_STREAM_HH
# define ELLE_SERIALIZE_DETAIL_STREAM_HH

# include <iosfwd>

namespace elle
{
  namespace serialize
  {
    namespace detail
    {

      void
      stream_write(std::ostream& out,
                   void const* data,
                   std::streamsize const size);

      void
      stream_read(std::istream& in,
                  void* data,
                  std::streamsize const size);

    }
  }
}

#endif
