#ifndef  ELLE_SERIALIZE_FOOTPRINT_HXX
# define ELLE_SERIALIZE_FOOTPRINT_HXX

# include <elle/assert.hh>
# include <elle/IOStream.hh>

# include "BinaryArchive.hh"

namespace elle
{
  namespace serialize
  {

    namespace detail
    {

      struct CounterStreamBuffer:
        public elle::StreamBuffer
      {
        size_t counter;
        char _buf[512];

        CounterStreamBuffer(): counter(0) {}

        virtual
        elle::WeakBuffer
        write_buffer()
        {
          return elle::WeakBuffer{
              _buf,
              sizeof(_buf) / sizeof(_buf[0])
          };
        }

        virtual
        elle::WeakBuffer
        read_buffer()
        {
          ELLE_ASSERT("Should not be called");
        }

        virtual
        void
        flush(unsigned int size)
        {
          counter += size;
        }
      };

    }

    template <typename Archive, typename T>
    size_t
    footprint(T&& value)
    {
      auto streambuf = new detail::CounterStreamBuffer;
      // takes owner ship XXX replace with unique_ptr<>&&
      elle::IOStream out(streambuf);
      Archive{out, std::forward<T>(value)};
      out.flush();
      return streambuf->counter;
    }

  }
}

#endif
